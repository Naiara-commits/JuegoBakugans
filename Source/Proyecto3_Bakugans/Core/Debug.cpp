#include "Core/Debug.h"

#include "Proyecto3_Bakugans.h"
#include "Engine/Engine.h"
#include "HAL/PlatformTime.h"

TAutoConsoleVariable<int32> CVarDebugAll(TEXT("game.Debug.All"), 0, TEXT("Turns on all debug."), ECVF_Cheat);
TAutoConsoleVariable<int32> CVarDebugMovement(TEXT("game.Debug.Movement"), 0, TEXT("Capsule, facing, velocity and movement mode."), ECVF_Cheat);
TAutoConsoleVariable<int32> CVarDebugInteraction(TEXT("game.Debug.Interaction"), 0, TEXT("Interact sweep, cooldown and pushes."), ECVF_Cheat);

namespace GameDebug
{
	bool IsOn(const TAutoConsoleVariable<int32>& CVar)
	{
		return CVar.GetValueOnGameThread() > 0 || CVarDebugAll.GetValueOnGameThread() > 0;
	}

	void Print(FName Key, const FString& Text, const FColor& Color, float Cooldown, float Duration)
	{
		// Last time each Key was printed (real time, so it also works while paused)
		static TMap<FName, double> LastPrintTime;

		const double Now = FPlatformTime::Seconds();
		double& Last = LastPrintTime.FindOrAdd(Key, -1.0e9);
		if (Now - Last < Cooldown)
		{
			return;
		}
		Last = Now;

		// The Key hash keeps one screen line per Key, so it updates instead of stacking
		UE_LOG(LogProyecto3_Bakugans, Log, TEXT("[%s] %s"), *Key.ToString(), *Text);
		if (GEngine)
		{
			GEngine->AddOnScreenDebugMessage(static_cast<uint64>(GetTypeHash(Key)), Duration, Color, Text);
		}
	}
}
