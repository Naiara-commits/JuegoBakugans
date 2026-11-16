// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Object.h"
#include "State.h"
#include "Idle.generated.h"

/**
 * 
 */
UCLASS()
class PROYECTO3_BAKUGANS_API UIdle : public UObject, public IState
{
	GENERATED_BODY()
	
public:
	virtual void OnStart() override;
	virtual void OnTick() override;
	virtual void OnEnd() override;
	
	
	
};
