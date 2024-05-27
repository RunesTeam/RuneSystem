

#pragma once

#include "CoreMinimal.h"
#include "RuneEffectApplicationMode.h"
#include "StackedRuneEffectApplicationMode.h"
#include "StatusRuneEffectApplicationMode.generated.h"


/**
 * Status Application Mode activates and applies the effect over a given target.
 * When certain time has passed, the effect is reverted and deactivated.
 */
UCLASS(Blueprintable, EditInlineNew, AutoExpandCategories = "StatusRuneEffectApplicationMode: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API UStatusRuneEffectApplicationMode : public URuneEffectApplicationMode
{
	GENERATED_BODY()

public:
	// Sets default values for this object's properties
	UStatusRuneEffectApplicationMode();

public:
	/**
	 * Handles how an effect is activated.
	 * Typically it would also handle the application of the effect.
	 *
	 * @param Handle Valid effect activation handle
	 */
	virtual void HandleEffectActivation(const FRuneEffectHandle& Handle);

	/**
	 * Handles how an effect is deactivated.
	 * Typically it would handle effect cancellation, interruption, etc.
	 *
	 * @param Handle Valid effect activation handle
	 */
	virtual void HandleEffectDeactivation(const FRuneEffectHandle& Handle);

public:
	/**
	 * How much time - in seconds - should the effect last.
	 * If negative it will last until reversion.
	 */
	UPROPERTY(EditAnywhere, Category = "StatusRuneEffectApplicationMode: General Settings")
	float Duration;

	/** Which policy the effect duration follows. Valid with non negative duration. */
	UPROPERTY(EditAnywhere, Category = "StatusRuneEffectApplicationMode: Stack Settings")
	EStackDurationPolicy StackDurationPolicy;

	/** How many effects can be stacked at a time. If set to 0, infinite stack count */
	UPROPERTY(EditAnywhere, Category = "StatusRuneEffectApplicationMode: Stack Settings", meta = (EditCondition = "StackDurationPolicy != EStackDurationPolicy::NONE"))
	uint32 MaxStackCount;

};


/** Status Application Mode Data */
UCLASS()
class RUNESYSTEM_API UStatusRuneEffectApplicationData : public UObject
{
	GENERATED_BODY()

public:
	UStatusRuneEffectApplicationData() = default;

public:
	UPROPERTY()
	FTimerHandle TimerHandle;

};