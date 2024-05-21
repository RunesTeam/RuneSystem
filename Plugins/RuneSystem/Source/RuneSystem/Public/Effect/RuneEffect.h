

#pragma once

#include "CoreMinimal.h"
#include "Utils/RuneUtils.h"
#include "RuneEffectPayload.h"
#include "RuneEffect.generated.h"


UCLASS(Abstract, Blueprintable, EditInlineNew, AutoExpandCategories = "RuneEffect: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API URuneEffect : public UObject
{
	GENERATED_BODY()
	DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEffectApplicationDelegate, const FRuneEffectPayload&, Payload);

public:
	// Sets default values for this object's properties
	URuneEffect();

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

protected:
	/**
	 * Manages the effect application to the specified AActor.
	 * (e.g. subtract X health points)
	 *
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will receive the effect application.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Apply"))
	void ReceiveApply(const FRuneEffectPayload& Payload);

	/**
	 * Manages the effect "undo" to the specified AActor.
	 * (e.g. recover X health points)
	 *
	 * @param target Actor which will receive the effect "undo".
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Revert"))
	void ReceiveRevert(const FRuneEffectPayload& Payload);

private:
	/**
	 * Invokes once Apply() with a given payload.
	 * Broadcasts all internal delegates.
	 * 
	 * @param Payload Used for targeting purposes.
	 */
	void SubmitApply(const FRuneEffectPayload& Payload);

	/**
	 * Invokes once Revert() with a given payload.
	 * Broadcasts all internal delegates.
	 *
	 * @param Payload Used for targeting purposes.
	 */
	void SubmitRevert(const FRuneEffectPayload& Payload);

	/**
	 * Manages the effect application to the specified AActor.
	 * (e.g. subtract X health points)
	 *
	 * @param Payload Used for targeting purposes.
	 */
	virtual void Apply(const FRuneEffectPayload& Payload);

	/**
	 * Manages the effect "undo" to the specified AActor.
	 * (e.g. recover X health points)
	 *
	 * @param Payload Used for targeting purposes.
	 */
	virtual void Revert(const FRuneEffectPayload& Payload);

	/**
	 * Whether or not the payload target has been filtered.
	 *
	 * The fact that something has been filtered out means
	 * that it should be discarded from the flow.
	 * Whether it is applying an effect or checking whether an
	 * effect can be applied to an actor, if it is filtered out,
	 * the given actor should NOT get any effect.
	 *
	 * @param Payload Payload used in the filtering process.
	 * @return If true, actor is filtered (discarded from the flow).
	 */
	UFUNCTION(BlueprintCallable)
	bool Filter(const FRuneEffectPayload& Payload) const;

	/**
	 * Initialize the activation of the effect with a given payload.
	 *
	 * @param Payload Structure containing activation data.
	 */
	UFUNCTION()
	FRuneEffectHandle Activate(const FRuneEffectPayload& Payload);

	/**
	 * Shuts down the lifetime of an activated effect given a valid handle.
	 *
	 * @param Handle Valid activation handle.
	 */
	UFUNCTION()
	void Deactivate(const FRuneEffectHandle& Handle);

public:
	/** Whether the effect should use a custom filter */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings")
	bool bOverrideFilter;

	/** Filter that overrides the global used filter */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", meta = (EditCondition = "overrideFilter==true", EditConditionHides))
	class URuneFilter* CustomFilter;

	/** Application Mode should be used */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", Instanced, NoClear, meta = (NoResetToDefault))
	class URuneEffectApplicationMode* ApplicationMode;

	/** Delegate invoked when a effect has been applied */
	UPROPERTY(BlueprintAssignable, Category = "RuneEffect: General Settings")
	FEffectApplicationDelegate OnEffectApplied;

	/** Delegate invoked when a effect has been reverted */
	UPROPERTY(BlueprintAssignable, Category = "RuneEffect: General Settings")
	FEffectApplicationDelegate OnEffectReverted;

protected:
	/**
	 * What faction this effect uses to filter.
	 * Used as a "modifiable constant".
	 * Although it could be done, this should not be modified dynamically.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RuneEffect: General Settings", meta = (Bitmask, BitmaskEnum = ERuneFilterFaction, DisplayAfter = "overrideFilter"))
	uint8 FilterFaction;

private:
	friend class URuneEffectApplicationMode;
	friend class URuneBaseComponent;
	friend class ARuneTangibleAgent;
	friend class URuneSystem;
};
