

#pragma once

#include "CoreMinimal.h"
#include "StackedRuneEffectApplicationMode.generated.h"


UENUM()
enum class EStackDurationPolicy : uint8
{
	/** Starts with the duration as if it was a new one */
	NONE			= 0		UMETA(DisplayName = "None"),

	/** Resets the duration timer of previous applications */
	RESETS			= 1		UMETA(DisplayName = "Resets"),

	/** Syncs up the duration timer with previous applications */
	SYNCRONIZE		= 2		UMETA(DisplayName = "Syncronize")
};

UENUM()
enum class EStackApplicationPolicy : uint8
{
	/** Starts the application process as if it was a new one */
	UNSYNCRONIZED	= 0		UMETA(DisplayName = "Unsyncronized"),

	/** Syncs up the application with previous effects */
	SYNCRONIZED		= 1		UMETA(DisplayName = "Syncronized")
};
