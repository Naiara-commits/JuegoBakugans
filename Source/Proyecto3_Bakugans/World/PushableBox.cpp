#include "World/PushableBox.h"

#include "Core/Debug.h"
#include "Components/BoxComponent.h"
#include "Components/StaticMeshComponent.h"
#include "DrawDebugHelpers.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

namespace
{
	/** Launch speed of a 50 kg box; the squash and stretch are measured against it */
	constexpr float ReferenceSpeed = 500.f;
}

APushableBox::APushableBox()
{
	// Tick only while the box is moving or wobbling (turned on in Interact, off when it settles)
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = false;

	// Physics body: a 100 cm box that overlaps the Interact channel, so the sweep sees it without stopping at it
	Body = CreateDefaultSubobject<UBoxComponent>(TEXT("Body"));
	Body->SetBoxExtent(FVector(50.f));
	Body->SetCollisionProfileName(TEXT("PhysicsActor"));
	Body->SetCollisionResponseToChannel(ECC_GameTraceChannel1, ECR_Overlap);
	Body->SetMobility(EComponentMobility::Movable);
	Body->SetSimulatePhysics(true);
	SetRootComponent(Body);

	// No rotation at all: it slides like a puzzle block and keeps its four sides aligned
	Body->BodyInstance.bLockXRotation = true;
	Body->BodyInstance.bLockYRotation = true;
	Body->BodyInstance.bLockZRotation = true;
	Body->BodyInstance.DOFMode = EDOFMode::SixDOF;

	// Visible mesh: engine cube by default (Blueprints can swap it), no collision of its own
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetupAttachment(Body);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (Cube.Succeeded())
	{
		Mesh->SetStaticMesh(Cube.Object);
	}
}

void APushableBox::BeginPlay()
{
	Super::BeginPlay();
	Body->SetMassOverrideInKg(NAME_None, MassKg, true);

	// Older Blueprints had "Mesh" as the physics root and may still carry those settings: the mesh is visual only
	Mesh->SetSimulatePhysics(false);
	Mesh->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshRestScale = Mesh->GetRelativeScale3D();
	MeshRestLocation = Mesh->GetRelativeLocation();
}

void APushableBox::Interact_Implementation(AActor* Interactor, const FHitResult& Hit)
{
	// Nothing to push if physics is off (for example, a box frozen in the level)
	if (!Interactor || !Body->IsSimulatingPhysics())
	{
		return;
	}

	// Straight along one of the box's own sides
	PushAxis = GetPushAxis(Interactor);
	const FVector Direction = GetActorTransform().TransformVectorNoScale(PushAxis);

	// Plain impulse (not a velocity change), so the mass is what makes heavy boxes slower
	Body->AddImpulse(Direction * PushStrength);

	// Squash on impact: the faster it leaves, the harder it squashes (light boxes squash more)
	const float LaunchSpeed = PushStrength / MassKg;
	const float Strength = HitSquash * FMath::Clamp(LaunchSpeed / ReferenceSpeed, 0.2f, 2.f);
	DeformSpeed -= Strength * FMath::Sqrt(Stiffness);
	SetActorTickEnabled(true);

	// Debug: cyan arrow with the push direction and the speed it leaves at
	if (GameDebug::IsOn(CVarDebugInteraction))
	{
		const FVector Location = GetActorLocation();
		DrawDebugDirectionalArrow(GetWorld(), Location, Location + Direction * 150.f, 30.f, FColor::Cyan, false, GameDebug::DrawTime, 0, 3.f);
		GameDebug::Print(GetFName(), FString::Printf(TEXT("%s: %.0f kg, sale a %.0f cm/s"), *GetName(), MassKg, LaunchSpeed), FColor::Cyan);
	}
}

void APushableBox::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	// Stretch while sliding: the target grows with the speed along the push axis
	const FVector Velocity = GetActorTransform().InverseTransformVectorNoScale(Body->GetPhysicsLinearVelocity());
	const float Speed = FMath::Abs(FVector::DotProduct(Velocity, PushAxis));
	const float Target = SlideStretch * FMath::Clamp(Speed / ReferenceSpeed, 0.f, 1.f);

	// Damped spring towards the target. It overshoots a little when the box stops (follow-through)
	// and eases in and out on its own. The step is capped so it stays stable at low frame rates
	const float Step = FMath::Min(DeltaSeconds, 1.f / 30.f);
	DeformSpeed += (Stiffness * (Target - Deform) - Damping * DeformSpeed) * Step;
	// Clamped so repeated pushes can never flip the mesh inside out
	Deform = FMath::Clamp(Deform + DeformSpeed * Step, -0.8f, 1.f);

	// Settled: snap back to rest and stop ticking
	if (Speed < 5.f && FMath::Abs(Deform) < 0.001f && FMath::Abs(DeformSpeed) < 0.01f)
	{
		Deform = 0.f;
		DeformSpeed = 0.f;
		SetActorTickEnabled(false);
	}
	ApplyDeform();
}

FVector APushableBox::GetPushAxis(const AActor* Interactor) const
{
	// Where the interactor is, seen from the box; the box moves the opposite way along the dominant axis
	const FVector FromInteractor = GetActorTransform().InverseTransformVectorNoScale(GetActorLocation() - Interactor->GetActorLocation());
	if (FMath::Abs(FromInteractor.X) >= FMath::Abs(FromInteractor.Y))
	{
		return FVector(FromInteractor.X >= 0.f ? 1.f : -1.f, 0.f, 0.f);
	}
	return FVector(0.f, FromInteractor.Y >= 0.f ? 1.f : -1.f, 0.f);
}

void APushableBox::ApplyDeform()
{
	// Longer (or shorter) along the push axis, thinner (or wider) on the other two: same volume
	const float Along = 1.f + Deform;
	const float Across = 1.f / FMath::Sqrt(FMath::Max(Along, 0.1f));
	const FVector Scale(PushAxis.X != 0.f ? Along : Across, PushAxis.Y != 0.f ? Along : Across, Across);
	Mesh->SetRelativeScale3D(MeshRestScale * Scale);

	// Keep the bottom of the mesh on the floor while its height changes (assumes the mesh fills the body)
	Mesh->SetRelativeLocation(MeshRestLocation + FVector(0.f, 0.f, Body->GetUnscaledBoxExtent().Z * (Scale.Z - 1.f)));
}
