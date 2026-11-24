// Copyright Epic Games, Inc. All Rights Reserved.


#include "Proyecto3_BakugansPlayerController.h"
#include "EnhancedInputSubsystems.h"
#include "Engine/LocalPlayer.h"
#include "InputMappingContext.h"
#include "Blueprint/UserWidget.h"
#include "Proyecto3_Bakugans.h"
#include "Widgets/Input/SVirtualJoystick.h"
#include "Combat/HealthComponent.h"
#include "UI/HeartsWidget.h"
#include "EngineUtils.h"
#include "GameFramework/DamageType.h"
#include "Kismet/GameplayStatics.h"

void AProyecto3_BakugansPlayerController::BeginPlay()
{
	Super::BeginPlay();

	// only spawn touch controls on local player controllers
	if (IsLocalPlayerController() && ShouldUseTouchControls())
	{
		// spawn the mobile controls widget
		MobileControlsWidget = CreateWidget<UUserWidget>(this, MobileControlsWidgetClass);

		if (MobileControlsWidget)
		{
			// add the controls to the player screen
			MobileControlsWidget->AddToPlayerScreen(0);

		} else {

			UE_LOG(LogProyecto3_Bakugans, Error, TEXT("Could not spawn mobile controls widget."));

		}

	}
}

void AProyecto3_BakugansPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// only add IMCs for local player controllers
	if (IsLocalPlayerController())
	{
		// Add Input Mapping Contexts
		if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(GetLocalPlayer()))
		{
			for (UInputMappingContext* CurrentContext : DefaultMappingContexts)
			{
				Subsystem->AddMappingContext(CurrentContext, 0);
			}

			// only add these IMCs if we're not using mobile touch input
			if (!ShouldUseTouchControls())
			{
				for (UInputMappingContext* CurrentContext : MobileExcludedMappingContexts)
				{
					Subsystem->AddMappingContext(CurrentContext, 0);
				}
			}
		}

#if !UE_BUILD_SHIPPING
		// Debug keys: plain key bindings, no input assets needed, gone in Shipping builds
		InputComponent->BindKey(EKeys::K, IE_Pressed, this, &AProyecto3_BakugansPlayerController::DebugDamagePlayer);
		InputComponent->BindKey(EKeys::J, IE_Pressed, this, &AProyecto3_BakugansPlayerController::DebugDamageOthers);
		InputComponent->BindKey(EKeys::H, IE_Pressed, this, &AProyecto3_BakugansPlayerController::DebugHealAll);
#endif
	}
}

void AProyecto3_BakugansPlayerController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);

	if (!IsLocalPlayerController() || !HeartsWidgetClass)
	{
		return;
	}

	// One hearts row for the whole session, pointed at whichever pawn we control
	if (!HeartsWidget)
	{
		HeartsWidget = CreateWidget<UHeartsWidget>(this, HeartsWidgetClass);
		if (!HeartsWidget)
		{
			return;
		}
		// Top-left corner at its natural size, with a margin from the screen edge
		HeartsWidget->AddToPlayerScreen();
		HeartsWidget->SetPositionInViewport(FVector2D(32.f, 32.f));
	}
	HeartsWidget->BindToHealth(InPawn ? InPawn->FindComponentByClass<UHealthComponent>() : nullptr);
}

bool AProyecto3_BakugansPlayerController::ShouldUseTouchControls() const
{
	// are we on a mobile platform? Should we force touch?
	return SVirtualJoystick::ShouldDisplayTouchInterface() || bForceTouchControls;
}

void AProyecto3_BakugansPlayerController::DebugDamagePlayer()
{
	// Same path as a real hit: ApplyDamage -> the health component loses a heart
	if (GetPawn())
	{
		UGameplayStatics::ApplyDamage(GetPawn(), 1.f, this, this, UDamageType::StaticClass());
	}
}

void AProyecto3_BakugansPlayerController::DebugDamageOthers()
{
	// Every actor with health except our own pawn (test dummies now, real enemies later)
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (*It != GetPawn() && It->FindComponentByClass<UHealthComponent>())
		{
			UGameplayStatics::ApplyDamage(*It, 1.f, this, GetPawn(), UDamageType::StaticClass());
		}
	}
}

void AProyecto3_BakugansPlayerController::DebugHealAll()
{
	for (TActorIterator<AActor> It(GetWorld()); It; ++It)
	{
		if (UHealthComponent* Health = It->FindComponentByClass<UHealthComponent>())
		{
			Health->ResetHealth();
		}
	}
}
