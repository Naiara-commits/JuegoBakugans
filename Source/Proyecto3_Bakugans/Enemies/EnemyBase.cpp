#include "Enemies/EnemyBase.h"

#include "Combat/HealthComponent.h"
#include "UI/HeartsWidget.h"
#include "Components/CapsuleComponent.h"
#include "Components/WidgetComponent.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyBase::AEnemyBase()
{
	Health = CreateDefaultSubobject<UHealthComponent>(TEXT("Health"));

	// Screen space: the hearts always face the camera and keep their size on screen
	HeartsWidget = CreateDefaultSubobject<UWidgetComponent>(TEXT("HeartsWidget"));
	HeartsWidget->SetupAttachment(GetCapsuleComponent());
	HeartsWidget->SetRelativeLocation(FVector(0.f, 0.f, 130.f));
	HeartsWidget->SetWidgetSpace(EWidgetSpace::Screen);
	HeartsWidget->SetDrawAtDesiredSize(true);

	// Enemies are driven by an AI controller, wherever they come from
	AutoPossessAI = EAutoPossessAI::PlacedInWorldOrSpawned;
}

void AEnemyBase::BeginPlay()
{
	// Components start here, so the widget exists and the health is filled after this call
	Super::BeginPlay();

	if (UHeartsWidget* Hearts = Cast<UHeartsWidget>(HeartsWidget->GetUserWidgetObject()))
	{
		Hearts->BindToHealth(Health);
	}
	Health->OnDeath.AddUniqueDynamic(this, &AEnemyBase::HandleDeath);
}

void AEnemyBase::HandleDeath(AActor* Killer)
{
	GetCharacterMovement()->DisableMovement();
	SetActorEnableCollision(false);
	HeartsWidget->SetVisibility(false);
	SetLifeSpan(DeathDelay);
}
