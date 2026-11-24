// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "Proyecto3_BakugansPlayerController.generated.h"

class UInputMappingContext;
class UUserWidget;
class UHeartsWidget;

/**
 *  Basic PlayerController class for a third person game
 *  Manages input mappings
 */
UCLASS(abstract)
class AProyecto3_BakugansPlayerController : public APlayerController
{
	GENERATED_BODY()
	
protected:

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category ="Input|Input Mappings")
	TArray<UInputMappingContext*> DefaultMappingContexts;

	/** Input Mapping Contexts */
	UPROPERTY(EditAnywhere, Category="Input|Input Mappings")
	TArray<UInputMappingContext*> MobileExcludedMappingContexts;

	/** Mobile controls widget to spawn */
	UPROPERTY(EditAnywhere, Category="Input|Touch Controls")
	TSubclassOf<UUserWidget> MobileControlsWidgetClass;

	/** Pointer to the mobile controls widget */
	UPROPERTY()
	TObjectPtr<UUserWidget> MobileControlsWidget;

	/** If true, the player will use UMG touch controls even if not playing on mobile platforms */
	UPROPERTY(EditAnywhere, Config, Category = "Input|Touch Controls")
	bool bForceTouchControls = false;

	/** Hearts row shown on the HUD (WBP_Hearts) */
	UPROPERTY(EditAnywhere, Category="UI")
	TSubclassOf<UHeartsWidget> HeartsWidgetClass;

	/** The hearts on screen, created once and re-bound to every pawn we possess */
	UPROPERTY()
	TObjectPtr<UHeartsWidget> HeartsWidget;

	/** Gameplay initialization */
	virtual void BeginPlay() override;

	/** Input mapping context setup */
	virtual void SetupInputComponent() override;

	/** Shows the hearts of the new pawn on the HUD */
	virtual void OnPossess(APawn* InPawn) override;

	/** Returns true if the player should use UMG touch controls */
	bool ShouldUseTouchControls() const;

	/** Debug keys (not in Shipping builds): K hurts the player, J hurts everyone else with health, H heals everyone */
	void DebugDamagePlayer();
	void DebugDamageOthers();
	void DebugHealAll();

};
