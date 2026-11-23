// Copyright Epic Games, Inc. All Rights Reserved.

#include "Proyecto3_BakugansCharacter.h"
#include "Engine/LocalPlayer.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/SpringArmComponent.h"
#include "GameFramework/Controller.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "Proyecto3_Bakugans.h"
#include "Core/Debug.h"
#include "Player/InteractionComponent.h"
#include "Combat/HealthComponent.h"
#include "DrawDebugHelpers.h"

AProyecto3_BakugansCharacter::AProyecto3_BakugansCharacter()
{
	// Set size for collision capsule
	GetCapsuleComponent()->InitCapsuleSize(42.f, 96.0f);
		
	// Don't rotate when the controller rotates. Let that just affect the camera.
	bUseControllerRotationPitch = false;
	bUseControllerRotationYaw = false;
	bUseControllerRotationRoll = false;

	// Configure character movement
	GetCharacterMovement()->bOrientRotationToMovement = true;
	GetCharacterMovement()->RotationRate = FRotator(0.0f, 500.0f, 0.0f);

	// Note: For faster iteration times these variables, and many more, can be tweaked in the Character Blueprint
	// instead of recompiling to adjust them
	GetCharacterMovement()->JumpZVelocity = 500.f;
	GetCharacterMovement()->AirControl = 0.35f;
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;
	GetCharacterMovement()->MinAnalogWalkSpeed = 20.f;
	GetCharacterMovement()->BrakingDecelerationWalking = 2000.f;
	GetCharacterMovement()->BrakingDecelerationFalling = 5000.f;
	GetCharacterMovement()->GravityScale = 1.5f;
	
	// Create a camera boom (pulls in towards the player if there is a collision)
	CameraBoom = CreateDefaultSubobject<USpringArmComponent>(TEXT("CameraBoom"));
	CameraBoom->SetupAttachment(RootComponent);
	CameraBoom->TargetArmLength = 400.0f;
	CameraBoom->bUsePawnControlRotation = true;

	// Create a follow camera
	FollowCamera = CreateDefaultSubobject<UCameraComponent>(TEXT("FollowCamera"));
	FollowCamera->SetupAttachment(CameraBoom, USpringArmComponent::SocketName);
	FollowCamera->bUsePawnControlRotation = false;

	InteractionComponent = CreateDefaultSubobject<UInteractionComponent>(TEXT("InteractionComponent"));
	HealthComponent = CreateDefaultSubobject<UHealthComponent>(TEXT("HealthComponent"));

	// Physics objects only move when interacted with, not when we walk into them
	GetCharacterMovement()->InitialPushForceFactor = 0.f;
	GetCharacterMovement()->PushForceFactor = 0.f;
	GetCharacterMovement()->TouchForceFactor = 0.f;
	GetCharacterMovement()->RepulsionForce = 0.f;

	// Note: The skeletal mesh and anim blueprint references on the Mesh component (inherited from Character) 
	// are set in the derived blueprint asset named ThirdPersonCharacter (to avoid direct content references in C++)
}

void AProyecto3_BakugansCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Set up action bindings
	if (UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>(PlayerInputComponent)) {

		// Sprinting: run while held, walk again on release
		if (SprintAction)
		{
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Started, this, &AProyecto3_BakugansCharacter::BeginRunning);
			EnhancedInputComponent->BindAction(SprintAction, ETriggerEvent::Completed, this, &AProyecto3_BakugansCharacter::StopRunning);
		}
		else
		{
			UE_LOG(LogProyecto3_Bakugans, Warning, TEXT("'%s' has no SprintAction assigned"), *GetNameSafe(this));
		}

		// Jumping
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Started, this, &ACharacter::Jump);
		EnhancedInputComponent->BindAction(JumpAction, ETriggerEvent::Completed, this, &ACharacter::StopJumping);

		// Moving
		EnhancedInputComponent->BindAction(MoveAction, ETriggerEvent::Triggered, this, &AProyecto3_BakugansCharacter::Move);
		EnhancedInputComponent->BindAction(MouseLookAction, ETriggerEvent::Triggered, this, &AProyecto3_BakugansCharacter::Look);

		// Looking
		EnhancedInputComponent->BindAction(LookAction, ETriggerEvent::Triggered, this, &AProyecto3_BakugansCharacter::Look);

		// Interacting
		if (InteractAction)
		{
			EnhancedInputComponent->BindAction(InteractAction, ETriggerEvent::Started, this, &AProyecto3_BakugansCharacter::DoInteract);
		}
		else
		{
			UE_LOG(LogProyecto3_Bakugans, Warning, TEXT("'%s' has no InteractAction assigned"), *GetNameSafe(this));
		}
	}
	else
	{
		UE_LOG(LogProyecto3_Bakugans, Error, TEXT("'%s' Failed to find an Enhanced Input component! This template is built to use the Enhanced Input system. If you intend to use the legacy system, then you will need to update this C++ file."), *GetNameSafe(this));
	}
}

void AProyecto3_BakugansCharacter::Move(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D MovementVector = Value.Get<FVector2D>();

	// route the input
	DoMove(MovementVector.X, MovementVector.Y);
}

void AProyecto3_BakugansCharacter::Look(const FInputActionValue& Value)
{
	// input is a Vector2D
	FVector2D LookAxisVector = Value.Get<FVector2D>();

	// route the input
	DoLook(LookAxisVector.X, LookAxisVector.Y);
}

void AProyecto3_BakugansCharacter::DoMove(float Right, float Forward)
{
	if (GetController() != nullptr)
	{
		// find out which way is forward
		const FRotator Rotation = GetController()->GetControlRotation();
		const FRotator YawRotation(0, Rotation.Yaw, 0);

		// get forward vector
		const FVector ForwardDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::X);

		// get right vector 
		const FVector RightDirection = FRotationMatrix(YawRotation).GetUnitAxis(EAxis::Y);

		// add movement 
		AddMovementInput(ForwardDirection, Forward);
		AddMovementInput(RightDirection, Right);
	}
}

void AProyecto3_BakugansCharacter::DoLook(float Yaw, float Pitch)
{
	if (GetController() != nullptr)
	{
		// add yaw and pitch input to controller
		AddControllerYawInput(Yaw);
		AddControllerPitchInput(Pitch);
	}
}

void AProyecto3_BakugansCharacter::DoJumpStart()
{
	// signal the character to jump
	Jump();
}

void AProyecto3_BakugansCharacter::DoJumpEnd()
{
	// signal the character to stop jumping
	StopJumping();
}

void AProyecto3_BakugansCharacter::DoInteract()
{
	InteractionComponent->TryInteract();
}

void AProyecto3_BakugansCharacter::Tick(float DeltaSeconds)
{
	Super::Tick(DeltaSeconds);

	if (GameDebug::IsOn(CVarDebugMovement))
	{
		DrawMovementDebug();
	}
	InteractionComponent->DrawDebug();
}

void AProyecto3_BakugansCharacter::DrawMovementDebug() const
{
	const UWorld* World = GetWorld();
	const FVector Location = GetActorLocation();
	const FVector Velocity = GetVelocity();

	// Capsule, facing (blue) and velocity (green)
	DrawDebugCapsule(World, Location, GetCapsuleComponent()->GetScaledCapsuleHalfHeight(), GetCapsuleComponent()->GetScaledCapsuleRadius(), GetActorQuat(), FColor::Silver);
	DrawDebugDirectionalArrow(World, Location, Location + GetActorForwardVector() * 100.f, 20.f, FColor::Blue);
	DrawDebugDirectionalArrow(World, Location, Location + Velocity * 0.25f, 20.f, FColor::Green);

	GameDebug::Print("Movement", FString::Printf(TEXT("%.0f / %.0f cm/s | %s"), Velocity.Size2D(), GetCharacterMovement()->MaxWalkSpeed, *GetCharacterMovement()->GetMovementName()), FColor::Green, 0.1f, 0.2f);
}

void AProyecto3_BakugansCharacter::BeginRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = MaxRunSpeed;

	if (GameDebug::IsOn(CVarDebugMovement))
	{
		GameDebug::Print("Sprint", FString::Printf(TEXT("Sprint ON: max %.0f cm/s"), MaxRunSpeed), FColor::Yellow);
	}
}

void AProyecto3_BakugansCharacter::StopRunning()
{
	GetCharacterMovement()->MaxWalkSpeed = WalkSpeed;

	if (GameDebug::IsOn(CVarDebugMovement))
	{
		GameDebug::Print("Sprint", FString::Printf(TEXT("Sprint OFF: max %.0f cm/s"), WalkSpeed), FColor::Yellow);
	}
}
