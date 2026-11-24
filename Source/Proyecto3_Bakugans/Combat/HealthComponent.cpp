#include "Combat/HealthComponent.h"

#include "Core/Debug.h"
#include "GameFramework/Actor.h"

UHealthComponent::UHealthComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UHealthComponent::BeginPlay()
{
	Super::BeginPlay();

	// Set here, not in the constructor, so a MaxHearts changed in a Blueprint is used.
	// It also broadcasts, so widgets that bound early show the starting hearts
	SetHearts(MaxHearts);

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

	SetHearts(CurrentHearts - 1);
	OnDamaged.Broadcast(this, DamageCauser);

	if (IsDead())
	{
		OnDeath.Broadcast(DamageCauser);
	}
}

void UHealthComponent::HealHearts(int32 Amount)
{
	if (Amount <= 0 || IsDead())
	{
		return;
	}
	SetHearts(FMath::Min(CurrentHearts + Amount, MaxHearts));
}

void UHealthComponent::ResetHealth()
{
	SetHearts(MaxHearts);
}

void UHealthComponent::SetHearts(int32 NewHearts)
{
	// Delta from the clamped value, so listeners get the real change
	const int32 OldHearts = CurrentHearts;
	CurrentHearts = FMath::Clamp(NewHearts, 0, MaxHearts);
	const int32 Delta = CurrentHearts - OldHearts;
	OnHeartsChanged.Broadcast(this, CurrentHearts, Delta);

	if (GameDebug::IsOn(CVarDebugCombat))
	{
		GameDebug::Print(GetOwner()->GetFName(), FString::Printf(TEXT("%s: %d/%d corazones%s"),
			*GetOwner()->GetName(), CurrentHearts, MaxHearts, IsDead() ? TEXT(" (muerto)") : TEXT("")), FColor::Red);
	}
}
