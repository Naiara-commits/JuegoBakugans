#include "Enemies/TestDummy.h"

#include "Combat/HealthComponent.h"
#include "UI/HeartsWidget.h"
#include "Components/StaticMeshComponent.h"
#include "Components/WidgetComponent.h"
#include "Engine/StaticMesh.h"
#include "UObject/ConstructorHelpers.h"

ATestDummy::ATestDummy()
{
	PrimaryActorTick.bCanEverTick = false;

	// Engine cube as the default body; Blueprints can swap it
	Mesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->SetCollisionProfileName(TEXT("BlockAllDynamic"));
	SetRootComponent(Mesh);
	static ConstructorHelpers::FObjectFinder<UStaticMesh> Cube(TEXT("/Engine/BasicShapes/Cube.Cube"));
	if (Cube.Succeeded())
	{
		Mesh->SetStaticMesh(Cube.Object);
	}

	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// Screen space: the hearts always face the camera and keep their size on screen
	HeartsWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HeartsWidget"));
	HeartsWidget->SetupAttachment(Mesh);
	HeartsWidget->SetRelativeLocation(FVector(0.f, 0.f, 110.f));
	HeartsWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HeartsWidget->SetDrawAtDesiredSize(true);
}

void ATestDummy::BeginPlay()
{
	// Components start here, so the widget exists and the health is filled after this call
	Super::BeginPlay();

	if (UHeartsWidget* Hearts = Cast<UHeartsWidget>(HeartsWidget->GetUserWidgetObject()))
	{
		Hearts->BindToHealth(Health);
	}
	Health->OnDeath.AddUniqueDynamic(this, &ATestDummy::HandleDeath);
}

void ATestDummy::HandleDeath(AActor* Killer)
{
	// Out of hearts: the dummy is gone for good
	Destroy();
}
