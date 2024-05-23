

#pragma once

#include "CoreMinimal.h"
#include "StackedRuneEffectApplicationMode.generated.h"


UENUM()
enum class EStackDurationPolicy : uint8
{
	/** Resets the duration timer of previous applications */
	RESETS			= 0		UMETA(DisplayName = "Resets"),

	/** Syncs up the duration timer with previous applications */
	SYNCRONIZE		= 1		UMETA(DisplayName = "Syncronize"),

	/** Starts with the duration as if it was a new one */
	NONE			= 2		UMETA(DisplayName = "None")
};

UENUM()
enum class EStackApplicationPolicy : uint8
{
	/** Syncs up the application with previous effects */
	SYNCRONIZED		= 0		UMETA(DisplayName = "Syncronized"),

	/** Starts the application process as if it was a new one */
	UNSYNCRONIZED	= 1		UMETA(DisplayName = "Unsyncronized")
};
