#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "InteractionComponent.generated.h"

/**
 * Finds what is in front of its owner and asks it to react through IInteractable.
 * It doesn't know what the reaction is (push, open, pick up...): each actor decides.
 */
UCLASS(ClassGroup = (Custom), meta = (BlueprintSpawnableComponent))
class PROYECTO3_BAKUGANS_API UInteractionComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UInteractionComponent();

	/** Sweeps in front of the owner and interacts with every IInteractable it finds. Returns true if anything reacted */
	UFUNCTION(BlueprintCallable, Category = "Interaction")
	bool TryInteract();

	/** Seconds left until the next interaction is allowed (0 = ready) */
	UFUNCTION(BlueprintPure, Category = "Interaction")
	float GetCooldownRemaining() const;

	/** Draws the reach and the cooldown for one frame. The owner calls it every Tick; it does nothing unless game.Debug.Interaction is on */
	void DrawDebug() const;

protected:
	/** How far in front of the owner the sweep ends */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (ClampMin = "0", Units = "cm"))
	float Range = 120.f;

	/** Radius of the sphere that is swept */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (ClampMin = "1", Units = "cm"))
	float Radius = 50.f;

	/** Minimum time between two presses, so the button can't be spammed */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction", meta = (ClampMin = "0", Units = "s"))
	float Cooldown = 0.35f;

	/** Interact trace channel: only interactive things respond to it (Overlap), so walls don't get in the way */
	UPROPERTY(EditDefaultsOnly, Category = "Interaction")
	TEnumAsByte<ECollisionChannel> TraceChannel = ECC_GameTraceChannel1;

private:
	/** Start and end of the sweep, in front of the owner */
	void GetSweep(FVector& OutStart, FVector& OutEnd) const;

	/** World time of the last accepted press (starts far in the past so the first press works) */
	double LastInteractTime = -1.0e9;
};
