// Fill out your copyright notice in the Description page of Project Settings.


#include "StateMachine.h"


// Sets default values for this component's properties
UStateMachine::UStateMachine()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}


// Called when the game starts
void UStateMachine::BeginPlay()
{
	Super::BeginPlay();
	currentState = states[0];
	currentState->OnStart();
	

}


// Called every frame
void UStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
	currentState->OnTick();
	// ...
}

template<class T>
inline void UStateMachine::ChangeState()
{
	currentState->OnEnd();
	currentState = FindState<T>();
	currentState->OnStart();
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
