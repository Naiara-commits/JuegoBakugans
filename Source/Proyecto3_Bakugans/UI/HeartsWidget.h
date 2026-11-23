#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "HeartsWidget.generated.h"

class UHealthComponent;
class UHorizontalBox;
class UImage;
class UTexture2D;

/**
 * A row of hearts that mirrors a UHealthComponent: one image per heart,
 * full hearts as they are, lost hearts grey and see-through.
 * Used by the player HUD and by the hearts floating over enemies.
 */
UCLASS(Abstract)
class PROYECTO3_BAKUGANS_API UHeartsWidget : public UUserWidget
{
	GENERATED_BODY()

public:
	/** Shows this health component and follows its changes from now on */
	UFUNCTION(BlueprintCallable, Category = "Hearts")
	void BindToHealth(UHealthComponent* Health);

protected:
	// No unbinding in NativeDestruct: it also runs when the widget only leaves the screen
	// (e.g. a hidden widget component), and dynamic delegates already skip destroyed widgets

	/** Creates the hearts row if the Blueprint doesn't have one */
	virtual void NativeOnInitialized() override;

	/** Row the hearts go into. Optional: a Horizontal Box with this name in the Blueprint is used, otherwise one is created */
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UHorizontalBox> HeartsBox;

	/** Image used for every heart */
	UPROPERTY(EditDefaultsOnly, Category = "Hearts")
	TObjectPtr<UTexture2D> HeartTexture;

	/** Size of each heart on screen */
	UPROPERTY(EditDefaultsOnly, Category = "Hearts")
	FVector2D HeartSize = FVector2D(48.f, 48.f);

	/** Tint of a lost heart: grey and see-through */
	UPROPERTY(EditDefaultsOnly, Category = "Hearts")
	FLinearColor LostTint = FLinearColor(0.3f, 0.3f, 0.3f, 0.4f);

private:
	/** Bound to the health component's OnHeartsChanged */
	UFUNCTION()
	void HandleHeartsChanged(UHealthComponent* HealthComponent, int32 CurrentHearts, int32 Delta);

	/** Keeps one image per heart and tints them: the first CurrentHearts are full, the rest lost */
	void Refresh(int32 CurrentHearts, int32 MaxHearts);

	UPROPERTY()
	TArray<TObjectPtr<UImage>> Hearts;

	TWeakObjectPtr<UHealthComponent> BoundHealth;
};
