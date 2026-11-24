#pragma once

#include "CoreMinimal.h"
#include "Enemies/EnemyBase.h"
#include "EnemyPatrol.generated.h"

/**
 * Goomba-style enemy: walks (or flies) straight ahead from where it spawns
 * and turns around every time it gets PatrolDistance away.
 */
UCLASS()
class PROYECTO3_BAKUGANS_API AEnemyPatrol : public AEnemyBase
{
	GENERATED_BODY()

public:
	AEnemyPatrol();

	/** Moves forward and turns around at the edge of the patrol */
	virtual void Tick(float DeltaSeconds) override;

protected:
	/** Remembers the spawn point and sets the patrol speed */
	virtual void BeginPlay() override;

	/** How far from the spawn point it goes before turning around */
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (ClampMin = "0", Units = "cm"))
	float PatrolDistance = 400.f;

	/** Walking (or flying) speed */
	UPROPERTY(EditAnywhere, Category = "Patrol", meta = (ClampMin = "0", Units = "cm/s"))
	float PatrolSpeed = 100.f;

	/** Flies instead of walking: no gravity, and the distance counts in 3D */
	UPROPERTY(EditAnywhere, Category = "Patrol")
	bool bFlying = false;

private:
	FVector SpawnPoint = FVector::ZeroVector;

	/** False right after turning, until it is back inside the patrol, so it doesn't spin at the edge */
	bool bCanTurn = true;
};
