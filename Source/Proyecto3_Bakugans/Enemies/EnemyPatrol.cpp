#include "Enemies/EnemyPatrol.h"

#include "Combat/HealthComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyPatrol::AEnemyPatrol()
{
	PrimaryActorTick.bCanEverTick = true;

	// The patrol turns the actor itself, so it must not follow the AI controller's rotation
	bUseControllerRotationYaw = false;
}

void AEnemyPatrol::BeginPlay()
{
	Super::BeginPlay();

	SpawnPoint = GetActorLocation();
	GetCharacterMovement()->MaxWalkSpeed = PatrolSpeed;
	GetCharacterMovement()->MaxFlySpeed = PatrolSpeed;
	if (bFlying)
	{
		GetCharacterMovement()->SetMovementMode(MOVE_Flying);
	}
}

void AEnemyPatrol::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (Health->IsDead())
	{
		return;
	}

	AddMovementInput(GetActorForwardVector());

	// Distance from the spawn point: flat for walkers, full 3D for flyers
	const float Distance = bFlying ? FVector::Dist(SpawnPoint, GetActorLocation()) : FVector::Dist2D(SpawnPoint, GetActorLocation());

	// Turn once at the edge, and only again after walking back inside (80% of the distance)
	if (bCanTurn && Distance >= PatrolDistance)
	{
		AddActorWorldRotation(FRotator(0.f, 180.f, 0.f));
		bCanTurn = false;
	}
	else if (Distance < PatrolDistance * 0.8f)
	{
		bCanTurn = true;
	}
}
