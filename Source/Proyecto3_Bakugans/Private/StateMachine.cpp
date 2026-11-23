// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine.h"


// Sets default values for this component's properties
UStateMachine::UStateMachine()
{
	states = std::vector<IState*>
	{
		NewObject<UIdle>(this, FName("IdleState")),
		NewObject<UWalking>(this, FName("WalkingState")),
		NewObject<URunning>(this, FName("RunningState")),
		NewObject<UDie>(this, FName("DieState")),
		NewObject<UJump>(this, FName("JumpState")),
		NewObject<ULand>(this, FName("LandState")),
		NewObject<UHit>(this, FName("HitState")),
	};

	stateDict = std::map<FString, IState*>{
		{ "Idle",states[0] },
		{ "Walking",states[1] },
		{ "Running",states[2] },
		{ "Die",states[3] },
		{ "Jump",states[4] },
		{ "Land",states[5] },
		{ "Hit",states[6] },

	};

	currentState = states[0];
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;


}


// Called when the game starts
void UStateMachine::BeginPlay()
{
	Super::BeginPlay();

	// ...

}


// Called every frame
void UStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	if (currentState)
		currentState->OnTick();
	// ...
}

IState* UStateMachine::FindState(FString state)
{
	std::map<FString, IState*>::iterator stateIt = stateDict.find(state);
	if (stateIt != stateDict.end())
	{
		return stateIt->second;
	}
	return nullptr;
}

void UStateMachine::ChangeState(FString state)
{
	IState* target = FindState(state);
	if (target) 
	{
		currentState->OnEnd();
		currentState = target;
		currentState->OnStart();
	}
}

template<class T>
inline void UStateMachine::ChangeState()
{
	IState* target = FindState<T>();
	if (target)
	{
		currentState->OnEnd();
		currentState = target;
		currentState->OnStart();
	}
}

template<class T>
IState* UStateMachine::FindState()
{
	for (auto it = states.begin(); it != states.end(); it++)
	{
		IState state = dynamic_cast<T*>(&(*it));
		if (state) {
			return state;
		}

	}
	return nullptr;
}
