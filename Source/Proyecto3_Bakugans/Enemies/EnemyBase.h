#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemyBase.generated.h"

class UHealthComponent;
class UWidgetComponent;

/**
 * Base for every enemy: a character with hearts, its hearts floating above it,
 * and a default death. Damage reaches it through ApplyDamage like anything else with health.
 * Each enemy only adds its own behaviour (patrol, chase, attack...).
 */
UCLASS(Abstract)
class PROYECTO3_BAKUGANS_API AEnemyBase : public ACharacter
{
	GENERATED_BODY()

public:
	AEnemyBase();

	/** Hearts of the enemy (attacks reach it through ApplyDamage; this is for reading or reacting) */
	UHealthComponent* GetHealthComponent() const { return Health; }

protected:
	/** Connects the floating hearts to the health and listens for death */
	virtual void BeginPlay() override;

	/** Default death: stops, hides its hearts and disappears after DeathDelay. Children can override it (animation, drops...) */
	UFUNCTION()
	virtual void HandleDeath(AActor* Killer);

	/** Hearts of the enemy. MaxHearts is set per enemy in its Blueprint */
	UPROPERTY(VisibleAnywhere, Category = "Enemy")
	TObjectPtr<UHealthComponent> Health;

	/** Hearts drawn above the enemy, always facing the camera. Its Widget Class is set in the Blueprint (WBP_Hearts) */
	UPROPERTY(VisibleAnywhere, Category = "Enemy")
	TObjectPtr<UWidgetComponent> HeartsWidget;

	/** Seconds between dying and disappearing */
	UPROPERTY(EditAnywhere, Category = "Enemy", meta = (ClampMin = "0.01", Units = "s"))
	float DeathDelay = 0.5f;
};
