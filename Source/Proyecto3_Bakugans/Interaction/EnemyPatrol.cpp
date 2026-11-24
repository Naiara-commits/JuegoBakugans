#include "EnemyPatrol.h"
#include "GameFramework/CharacterMovementComponent.h"

AEnemyPatrol::AEnemyPatrol()
{
	PrimaryActorTick.bCanEventTick = true;
	
	AutoPossessAI = EAutoPossessAI::PlaceInWorldOrSpawned;
}

void AEnemyPatrol::BeginPlay()
{
		Super::BeginPlay();
		spawnPoint = GetActorLocation();
		
		if(UCharacterMovementComponent* MoveComp = GetCharacterMovement())
		{
			MoveComp()->MaxWalkSpeed = PatrolSpeed;
			MoveComp()->MaxFlySpeed = PatrolSpeed;
			if (bFlyPatrolling)
			{
				MoveComp->SetMovementMode(MOVE_Flying);
			}

		}
}

void AEnemyPatrol::Tick(float DeltaTime)
{
		Super::Tick(DeltaTime);
		AddMovementInput(GetActorForwardVector(), 1.0f);
		const float distanceCovered = bFlyPatrolling;
		if (bFlyPatrolling)
		{
			FVector::Dist(spawnPoint, GetActorLocation());
		}
		else if (!bFlyPatrolling)
		{
			FVector::Dist2D(spawnPoint, GetActorLocation());
		}
		
		if(distanceCovered >= PatrolDistance && bCanTurn)
		{
			Turn();
			bCanTurn = false;
		}
		else if(distanceCovered < PatrolDistance * 0.8f)
		{
			bCanTurn = true;
		}
}

void AEnemyPatrol::Turn()
{
		FRotator actualRot = GetActorRotation();
		actualRot.yaw += 180.0f;
		SetActorRotation(actualRot);
}