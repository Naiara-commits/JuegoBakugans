#pragma once

#include "CoreMinimal.h"
#include "Engine/HitResult.h"
#include "UObject/Interface.h"
#include "Interactable.generated.h"

UINTERFACE(MinimalAPI, BlueprintType)
class UInteractable : public UInterface
{
	GENERATED_BODY()
};

/**
 * Anything that reacts to the player's interact button: boxes, levers, buttons...
 * Implement it in C++ (Interact_Implementation) or in a Blueprint (event Interact).
 */
class PROYECTO3_BAKUGANS_API IInteractable
{
	GENERATED_BODY()

public:
	/** Called once per press. Interactor is who pressed the button; Hit is where the sweep touched this actor */
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Interaction")
	void Interact(AActor* Interactor, const FHitResult& Hit);
};
