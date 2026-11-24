#include "Player/InteractionComponent.h"

#include "Core/Debug.h"
#include "Interaction/Interactable.h"
#include "DrawDebugHelpers.h"
#include "Engine/World.h"
#include "GameFramework/Actor.h"

UInteractionComponent::UInteractionComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

bool UInteractionComponent::TryInteract()
{
	AActor* Owner = GetOwner();
	UWorld* World = GetWorld();
	if (!Owner || !World)
	{
		return false;
	}

	const bool bDebug = GameDebug::IsOn(CVarDebugInteraction);

	// Ignore the press while the cooldown is running
	if (const float Remaining = GetCooldownRemaining(); Remaining > 0.f)
	{
		if (bDebug)
		{
			GameDebug::Print("Interaction", FString::Printf(TEXT("En cooldown: %.2f s"), Remaining), FColor::Yellow);
		}
		return false;
	}
	LastInteractTime = World->GetTimeSeconds();

	// Sphere sweep in front of the owner; Multi returns every overlap along the way, not just the first
	FVector Start, End;
	GetSweep(Start, End);

	TArray<FHitResult> Hits;
	const FCollisionQueryParams Params(SCENE_QUERY_STAT(Interaction), false, Owner);
	World->SweepMultiByChannel(Hits, Start, End, FQuat::Identity, TraceChannel, FCollisionShape::MakeSphere(Radius), Params);

	// Each actor reacts only once, even if the sweep touches several of its components
	TSet<AActor*> Handled;
	for (const FHitResult& Hit : Hits)
	{
		AActor* Target = Hit.GetActor();
		if (!Target || !Target->Implements<UInteractable>() || Handled.Contains(Target))
		{
			continue;
		}
		Handled.Add(Target);
		IInteractable::Execute_Interact(Target, Owner, Hit);
	}

	// Debug: green spheres if something reacted, red if not, plus a summary on screen
	if (bDebug)
	{
		const FColor Color = Handled.Num() > 0 ? FColor::Green : FColor::Red;
		DrawDebugSphere(World, Start, Radius, 12, Color, false, GameDebug::DrawTime);
		DrawDebugSphere(World, End, Radius, 12, Color, false, GameDebug::DrawTime);
		GameDebug::Print("Interaction", FString::Printf(TEXT("Interactuar: %d impactos, %d interactivos"), Hits.Num(), Handled.Num()), Color);
	}
	return Handled.Num() > 0;
}

float UInteractionComponent::GetCooldownRemaining() const
{
	const UWorld* World = GetWorld();
	return World ? FMath::Max(0.f, Cooldown - static_cast<float>(World->GetTimeSeconds() - LastInteractTime)) : 0.f;
}

void UInteractionComponent::DrawDebug() const
{
	const AActor* Owner = GetOwner();
	if (!Owner || !GameDebug::IsOn(CVarDebugInteraction))
	{
		return;
	}

	// Reach: grey when ready, red with a countdown above the owner while on cooldown
	FVector Start, End;
	GetSweep(Start, End);
	const float Remaining = GetCooldownRemaining();
	const FColor Color = Remaining > 0.f ? FColor::Red : FColor(160, 160, 160);
	DrawDebugSphere(GetWorld(), End, Radius, 8, Color);
	DrawDebugLine(GetWorld(), Start, End, Color);

	if (Remaining > 0.f)
	{
		DrawDebugString(GetWorld(), Owner->GetActorLocation() + FVector(0.f, 0.f, 120.f), FString::Printf(TEXT("%.2f s"), Remaining), nullptr, Color, -1.f, true);
	}
}

void UInteractionComponent::GetSweep(FVector& OutStart, FVector& OutEnd) const
{
	const AActor* Owner = GetOwner();
	const FVector Forward = Owner->GetActorForwardVector();
	OutStart = Owner->GetActorLocation() + Forward * Radius;
	OutEnd = Owner->GetActorLocation() + Forward * Range;
}
