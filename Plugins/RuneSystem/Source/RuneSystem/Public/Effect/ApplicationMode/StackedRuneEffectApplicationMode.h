

#pragma once

#include "CoreMinimal.h"
#include "StackedRuneEffectApplicationMode.generated.h"


UENUM()
enum class EStackDurationPolicy : uint8
{
	/** Starts with the duration as if it was a new one. Does nothing on max stack count reached */
	NONE			= 0		UMETA(DisplayName = "None"),

	/** Resets the duration timer of previous applications. Resets duration on max stack count reached. */
	RESETS			= 1		UMETA(DisplayName = "Resets"),

	/** Syncs up the duration timer with previous applications. Does nothing on max stack count reached */
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


USTRUCT(BlueprintType)
struct FStackApplicationSpecification
{
	GENERATED_BODY()

	/** Which policy the effect duration follows. Valid with non negative duration. */
	UPROPERTY(EditAnywhere)
	EStackDurationPolicy StackDurationPolicy = EStackDurationPolicy::NONE;

	/** Which policy the effect application follows */
	UPROPERTY(EditAnywhere)
	EStackApplicationPolicy StackApplicationPolicy = EStackApplicationPolicy::UNSYNCRONIZED;

	/** How many effects can be stacked at a time. If set to 0, infinite stack count */
	UPROPERTY(EditAnywhere, meta = (EditCondition = "StackDurationPolicy != EStackDurationPolicy::NONE || StackApplicationPolicy != EStackApplicationPolicy::UNSYNCRONIZED"))
	uint32 MaxStackCount = 0u;
};
