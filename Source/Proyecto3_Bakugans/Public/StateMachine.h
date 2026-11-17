// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "State.h"
#include "Idle.h"
#include "Die.h"
#include "Hit.h"
#include "Jump.h"
#include "Land.h"
#include "Running.h"
#include "Walking.h"
#include <vector>
#include "StateMachine.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class PROYECTO3_BAKUGANS_API UStateMachine : public UActorComponent
{
	GENERATED_BODY()
private:
	IState* currentState;
	std::vector<IState*> states;
	UActor* parentActor;
public:	
	// Sets default values for this component's properties
	UStateMachine();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	template <class T = IState> void ChangeState();
	template <class T = IState> IState* FindState();
	
};


