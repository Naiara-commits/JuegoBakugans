#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "TestDummy.generated.h"

class UHealthComponent;
class UStaticMeshComponent;
class UWidgetComponent;

/**
 * Test enemy: a cube with health and its hearts floating above it.
 * When it dies it hides, and after RespawnTime it comes back with full hearts,
 * so damage can be tested again and again.
 */
UCLASS()
class PROYECTO3_BAKUGANS_API ATestDummy : public AActor
{
	GENERATED_BODY()

public:
	ATestDummy();

	/** Hearts of the dummy (attacks reach it through ApplyDamage; this is for reading or reacting) */
	UHealthComponent* GetHealthComponent() const { return Health; }

protected:
	/** Connects the floating hearts to the health and listens for death */
	virtual void BeginPlay() override;

	/** Clears the respawn timer */
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	/** Body of the dummy (an engine cube by default) */
	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Hearts of the dummy */
	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UHealthComponent> Health;

	/** Hearts drawn above the dummy, always facing the camera. Its Widget Class is set in the Blueprint (WBP_Hearts) */
	UPROPERTY(VisibleAnywhere, Category = "Dummy")
	TObjectPtr<UWidgetComponent> HeartsWidget;

	/** Seconds hidden after dying before it comes back */
	UPROPERTY(EditAnywhere, Category = "Dummy", meta = (ClampMin = "0", Units = "s"))
	float RespawnTime = 2.f;

private:
	/** Bound to Health->OnDeath: hide and start the respawn timer */
	UFUNCTION()
	void HandleDeath(AActor* Killer);

	/** Show again with full hearts */
	void Respawn();

	FTimerHandle RespawnTimer;
};
