#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "HealthComponent.generated.h"

class UHealthComponent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_ThreeParams(FHeartsChangedSignature, UHealthComponent*, HealthComponent, int32, CurrentHearts, int32, Delta);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FDamagedSignature, UHealthComponent*, HealthComponent, AActor*, DamageCauser);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FDeathSignature, AActor*, Killer);

/**
 * Health counted in whole hearts. It listens to the owner's damage
 * (UGameplayStatics::ApplyDamage): every hit costs one heart, whatever its amount.
 * Any attack only has to call ApplyDamage on what it hits; it never needs to know about this component.
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

	/** Back to full hearts, even if dead (respawn) */
	UFUNCTION(BlueprintCallable, Category = "Health")
	void ResetHealth();

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetCurrentHearts() const { return CurrentHearts; }

	UFUNCTION(BlueprintPure, Category = "Health")
	int32 GetMaxHearts() const { return MaxHearts; }

	UFUNCTION(BlueprintPure, Category = "Health")
	bool IsDead() const { return CurrentHearts <= 0; }

	/** Fired on every change, including the starting fill; Delta is negative on damage and positive on healing */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FHeartsChangedSignature OnHeartsChanged;

	/** Fired on every hit that costs a heart, with what caused it: hit reactions (flash, knockback, sound) */
	UPROPERTY(BlueprintAssignable, Category = "Health")
	FDamagedSignature OnDamaged;

	/** Fired once, when the last heart is lost, with what dealt the final hit */
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

	/** Sets CurrentHearts, notifies listeners and prints it with game.Debug.Combat */
	void SetHearts(int32 NewHearts);

	int32 CurrentHearts = 0;
};
