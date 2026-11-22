#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "VHealthComponent.generated.h"

// Delegados actualizados a enteros
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FWhenDeathEvent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FWhenHeartsChangedEvent, class UVHealthComponent*, HealthComp, int32, CurrentHearts, int32, HeartsDelta);

UCLASS(meta=(BlueprintSpawnableComponent) )
class PROYECTO3_BAKUGANS_API UHealthComponent : public UActorComponent
{
    GENERATED_BODY()

public:	
    UHealthComponent();

protected:
    virtual void BeginPlay() override;

    // Número de corazones máximos con los que empieza el personaje
    UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Corazones")
    int32 MaxHearts;

    // Corazones actuales
    UPROPERTY(BlueprintReadOnly, Category = "Corazones")
    int32 CurrentHearts;

public:	
    UPROPERTY(BlueprintAssignable, Category = "Eventos")
    FWhenDeathEvent OnDeath;

    UPROPERTY(BlueprintAssignable, Category = "Eventos")
    FWhenHeartsChangedEvent OnHeartsChanged;

    // Función para recuperar corazones
    UFUNCTION(BlueprintCallable, Category = "Corazones")
    void HealHearts(int32 HeartsToRestore);

    UFUNCTION()
    void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const class UDamageType* DamageType, class AController* InstigatedBy, AActor* DamageCauser);
};
