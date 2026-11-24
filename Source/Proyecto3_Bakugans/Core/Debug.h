#pragma once

#include "CoreMinimal.h"
#include "HAL/IConsoleManager.h"

// Debug switches for the console (the key under Esc): 1 on, 0 off. game.Debug.All turns everything on.
// ECVF_Cheat: they can't be changed from the console in Shipping builds.
extern PROYECTO3_BAKUGANS_API TAutoConsoleVariable<int32> CVarDebugAll;
extern PROYECTO3_BAKUGANS_API TAutoConsoleVariable<int32> CVarDebugMovement;
extern PROYECTO3_BAKUGANS_API TAutoConsoleVariable<int32> CVarDebugInteraction;
extern PROYECTO3_BAKUGANS_API TAutoConsoleVariable<int32> CVarDebugCombat;

namespace GameDebug
{
	/** Seconds that the lines of a one-off event (a press, a push) stay on screen */
	constexpr float DrawTime = 1.5f;

	/** True if this switch or game.Debug.All is on */
	PROYECTO3_BAKUGANS_API bool IsOn(const TAutoConsoleVariable<int32>& CVar);

	/** Message on screen and in the log. A message with the same Key replaces the previous one
	 *  instead of stacking, and it isn't repeated until Cooldown seconds have passed */
	PROYECTO3_BAKUGANS_API void Print(FName Key, const FString& Text, const FColor& Color = FColor::White, float Cooldown = 0.f, float Duration = 2.f);
}
