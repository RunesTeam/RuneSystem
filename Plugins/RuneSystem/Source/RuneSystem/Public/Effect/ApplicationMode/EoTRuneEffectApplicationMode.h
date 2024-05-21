

#pragma once

#include "CoreMinimal.h"
#include "RuneEffectApplicationMode.h"
#include "EoTRuneEffectApplicationMode.generated.h"


/**
 * Effect Over Time (EoT) Application Mode activates a timer that applies the effect over a given target.
 * Deactivation ensures the timer is cleared on finished, interrupted or canceled.
 */
UCLASS(Blueprintable, EditInlineNew, AutoExpandCategories = "EoTRuneEffectApplicationMode: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API UEoTRuneEffectApplicationMode : public URuneEffectApplicationMode
{
	GENERATED_BODY()

public:
	// Sets default values for this object's properties
	UEoTRuneEffectApplicationMode();

#if WITH_EDITOR
	/**
	 * Called by the editor to query whether a property of this object is allowed to be modified.
	 * The property editor uses this to disable controls for properties that should not be changed.
	 * When overriding this function you should always call the parent implementation first.
	 *
	 * @param InProperty The property to query
	 *
	 * @return true if the property can be modified in the editor, otherwise false
	 */
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

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
	UPROPERTY(EditAnywhere, Category = "EoTRuneEffectApplicationMode: General Settings")
	float Duration;

	/** How many times should the effect be applied. */
	UPROPERTY(EditAnywhere, Category = "EoTRuneEffectApplicationMode: General Settings", meta = (EditCondition = "Duration > 0.0f", EditConditionHides))
	uint32 Ticks;

	/**
	 * Whether the ticks should be adjusted to the duration start-end interval or just distributed in time.
	 * - If true:	|__|__|__|__|
	 * - If false:	_|_|_|_|_|_
	 */
	UPROPERTY(EditAnywhere, Category = "EoTRuneEffectApplicationMode: General Settings", meta = (EditCondition = "Duration > 0.0f", EditConditionHides))
	bool TrimTickDistribution;

	/**
	 * Time window - in seconds - between consecutive ticks.
	 * Necessary for effects with negative (undefined) duration.
	 */
	UPROPERTY(EditAnywhere, Category = "EoTRuneEffectApplicationMode: General Settings", meta = (EditCondition="Duration <= 0.0f", EditConditionHides))
	float TickRate;

};


/** Effect Over Time (EoT) Application Mode Data */
UCLASS()
class RUNESYSTEM_API UEoTRuneEffectApplicationData : public UObject
{
	GENERATED_BODY()

public:
	UEoTRuneEffectApplicationData() = default;

public:
	UPROPERTY()
	FTimerHandle TickTimerHandle;

	UPROPERTY()
	FTimerHandle DeactivationTimerHandle;

};
