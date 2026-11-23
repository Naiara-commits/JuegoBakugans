#include "Combat/HealthComponent.h"

#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set here, not in the constructor, so a MaxHearts changed in a Blueprint is used
	CurrentHearts = MaxHearts;

	if (AActor* Owner = GetOwner())
	{
		Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
	}
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser)
{
	// Any real hit costs exactly one heart; once dead, damage is ignored
	if (Damage <= 0.f || IsDead())
	{
		return;
	}

	--CurrentHearts;
	OnHeartsChanged.Broadcast(this, CurrentHearts, -1);

	if (IsDead())
	{
		OnDeath.Broadcast();
	}
}

void UHealthComponent::HealHearts(int32 Amount)
{
	if (Amount <= 0 || IsDead())
	{
		return;
	}

	const int32 Before = CurrentHearts;
	CurrentHearts = FMath::Min(CurrentHearts + Amount, MaxHearts);

	// Only notify if something actually changed (already full means no change)
	if (CurrentHearts != Before)
	{
		OnHeartsChanged.Broadcast(this, CurrentHearts, CurrentHearts - Before);
	}
}
