#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHeartsChangedSignature, UHealthComponent*, HealthComponent, int32, CurrentHearts, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FDeathSignature);

/**
 * Health counted in whole hearts. It listens to the owner's damage
 * (UGameplayStatics::ApplyDamage): every hit costs one heart, whatever its amount.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROYECTO3_BAKUGANS_API UHealthComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UHealthComponent();

	/** Restores hearts up to MaxHearts. Does nothing once dead */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void HealHearts(int32 Amount);

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetCurrentHearts() const { return CurrentHearts; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return CurrentHearts <= 0; }

	/** Fired on every change; Delta is negative on damage and positive on healing */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FHeartsChangedSignature OnHeartsChanged;

	/** Fired once, when the last heart is lost */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FDeathSignature OnDeath;

protected:
	/** Fills the hearts and starts listening to the owner's damage */
	virtual void BeginPlay() override;

	/** Hearts at the start */
	UPROPERTY(EditDefaultsOnly, Category = "Health", meta = (ClampMin = "1"))
	int32 MaxHearts = 3;

private:
	/** Bound to the owner's OnTakeAnyDamage */
	UFUNCTION()
	void HandleTakeAnyDamage(AActor* DamagedActor, float Damage, const UDamageType* DamageType, AController* InstigatedBy, AActor* DamageCauser);

	int32 CurrentHearts = 0;
};
