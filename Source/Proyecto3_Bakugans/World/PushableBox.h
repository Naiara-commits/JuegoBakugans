#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interaction/Interactable.h"
#include "PushableBox.generated.h"

class UBoxComponent;
class UStaticMeshComponent;

/**
 * Physics box that slides straight along one of its four sides when the player interacts with it.
 * Heavier boxes (MassKg) move less with the same push.
 * The visible mesh squashes on the hit, stretches while sliding and wobbles when it stops;
 * the physics body never deforms, so collision stays exact.
 */
UCLASS()
class PROYECTO3_BAKUGANS_API APushableBox : public AActor, public IInteractable
{
	GENERATED_BODY()

public:
	APushableBox();

	/** IInteractable: pushes the box straight away from the side the interactor is on */
	virtual void Interact_Implementation(AActor* Interactor, const FHitResult& Hit) override;

	/** Only runs while the box is wobbling or sliding; it turns itself off when the box settles */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Applies MassKg to the physics body */
	virtual void BeginPlay() override;

	/** Physics body: slides and collides, never deforms */
	UPROPERTY(VisibleAnywhere, Category = "Push")
	TObjectPtr<UBoxComponent> Body;

	/** What you see (an engine cube by default). Only this squashes and stretches */
	UPROPERTY(VisibleAnywhere, Category = "Push")
	TObjectPtr<UStaticMeshComponent> Mesh;

	/** Mass of the box. With the same push, more mass means less movement */
	UPROPERTY(EditAnywhere, Category = "Push", meta = (ClampMin = "1", Units = "kg"))
	float MassKg = 50.f;

	/** Push impulse (kg·cm/s): 25000 sends a 50 kg box off at 500 cm/s */
	UPROPERTY(EditAnywhere, Category = "Push", meta = (ClampMin = "0"))
	float PushStrength = 25000.f;

	/** How much the hit squashes the box. Scaled by launch speed, so light boxes squash more */
	UPROPERTY(EditAnywhere, Category = "Feel", meta = (ClampMin = "0", ClampMax = "0.9"))
	float HitSquash = 0.3f;

	/** How much the box stretches while sliding fast */
	UPROPERTY(EditAnywhere, Category = "Feel", meta = (ClampMin = "0", ClampMax = "0.9"))
	float SlideStretch = 0.15f;

	/** Spring stiffness of the wobble: higher is a faster, tighter wobble (capped so the spring stays stable at 30 fps) */
	UPROPERTY(EditAnywhere, Category = "Feel", meta = (ClampMin = "1", ClampMax = "1000"))
	float Stiffness = 300.f;

	/** Spring damping of the wobble: higher stops the wobble sooner (capped so the spring stays stable at 30 fps) */
	UPROPERTY(EditAnywhere, Category = "Feel", meta = (ClampMin = "0", ClampMax = "40"))
	float Damping = 12.f;

private:
	/** Local axis (±X or ±Y) of the box side opposite to the interactor */
	FVector GetPushAxis(const AActor* Interactor) const;

	/** Squashes or stretches the mesh along PushAxis by Deform, keeping its volume and its bottom on the floor */
	void ApplyDeform();

	/** Local axis of the last push */
	FVector PushAxis = FVector::ForwardVector;

	/** 0 = rest, below 0 = squashed, above 0 = stretched (along PushAxis) */
	float Deform = 0.f;

	/** How fast Deform is changing (spring velocity) */
	float DeformSpeed = 0.f;

	/** Mesh scale and location set in the editor; the deform is applied on top of them */
	FVector MeshRestScale = FVector::OneVector;
	FVector MeshRestLocation = FVector::ZeroVector;
};
