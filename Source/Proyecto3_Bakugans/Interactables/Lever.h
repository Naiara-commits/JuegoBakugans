// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Interactables/Interactable.h"
#include "Lever.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FOnLeverSwitched, int, LeverID);

UCLASS()
class PROYECTO3_BAKUGANS_API ALever : public AActor, public IInteractable
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ALever();
	UPROPERTY(EditAnywhere)
	int id;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	UPROPERTY(BlueprintAssignable, Category = "Events")
	FOnLeverSwitched OnLeverSwitched;
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	void Interact() override;
	
	
};
