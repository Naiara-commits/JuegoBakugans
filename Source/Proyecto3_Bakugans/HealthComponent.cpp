#include "HealthComponent.h"

UHealthComponent::UHealthComponent()
{
    PrimaryComponentTick.bCanEverTick = false;

    // Inicializamos con 3 corazones por defecto
    MaxHearts = 3;
    CurrentHearts = MaxHearts;
}

void UHealthComponent::BeginPlay()
{
    Super::BeginPlay();

    AActor* Owner = GetOwner();
    if (Owner)
    {
        Owner->OnTakeAnyDamage.AddDynamic(this, &UHealthComponent::HandleTakeAnyDamage);
    }
}

void UHealthComponent::HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser)
{
    if (Damage <= 0.0f || CurrentHearts <= 0)
    {
        return;
    }

    // Por defecto, cualquier golpe resta 1 corazón. 
    int32 HeartsToLose = 1;

    // Restamos el corazón asegurando que no baje de 0
    CurrentHearts = FMath::Clamp(CurrentHearts - HeartsToLose, 0, MaxHearts);
    
    // Notificamos el cambio (enviamos el delta en negativo)
    OnHeartsChanged.Broadcast(this, CurrentHearts, -HeartsToLose);

    if (CurrentHearts <= 0)
    {
        OnDeath.Broadcast();
    }
}

void UHealthComponent::HealHearts(int32 HeartsToRestore)
{
    if (HeartsToRestore <= 0 || CurrentHearts <= 0)
    {
        return;
    }

    CurrentHearts = FMath::Clamp(CurrentHearts + HeartsToRestore, 0, MaxHearts);
    OnHeartsChanged.Broadcast(this, CurrentHearts, HeartsToRestore);
}
