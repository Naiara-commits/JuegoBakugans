#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "EnemigoPatrulla.generated.h"

UCLASS()
class PROYECTO3_BAKUGANS_API AEnemyPatrol : public ACharacter
{
		GENERATED_BODY();
	
	public:
		AEnemyPatrol();
	
	protected:
		virtual void BeginPlay() override;
		
	public:
		virtual void Tick(float DeltaTime) override;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
		float PatrolDistance = 400.0f;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol")
		float PatrolSpeed = 100.0f;
		
		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol&Air")
		bool bFlyPatrolling = true;

		UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol&Air", meta = (EditCondition = "bFlyPatrolling"))
		float FlyingAngle = 0.0f;

	private:
		FVector spawnPoint;
		
		bool bCanTurn = true;
		
		void Turn();
		