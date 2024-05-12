

#pragma once

#include "CoreMinimal.h"
#include "Utils/RuneUtils.h"
#include "RuneEffectPayload.h"
#include "RuneEffect.generated.h"


class AActor;
class URuneFilter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEffectApplicationDelegate, const FRuneEffectPayload&, Payload);

UCLASS(Abstract, Blueprintable, EditInlineNew, AutoExpandCategories = "RuneEffect: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API URuneEffect : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
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

public:
	/**
	 * Gets the filter that it is going to be used for filtering.
	 * If OverrideFilter is true, CustomFilter will be returned.
	 *
	 * @return Used filter, either it is CustomFilter or the global filter.
	 */
	UFUNCTION(BlueprintCallable)
	const URuneFilter* GetUsedFilter() const;

	/**
	 * Get the cached instigator of the effect.
	 * It could be nullptr.
	 * 
	 * @return Instigator.
	 */
	UFUNCTION(BlueprintCallable)
	AController* GetInstigator() const;

	/**
	 * Sets the cached instigator of the effect.
	 * 
	 * @param filter Instigator filter
	 */
	UFUNCTION(BlueprintCallable)
	void SetInstigator(AController* instigator);

	/**
	 * Get the cached filter used by the instigator of the effect.
	 * It could be nullptr.
	 * 
	 * @return Instigator filter.
	 */
	UFUNCTION(BlueprintCallable)
	const URuneFilter* GetInstigatorFilter() const;

	/**
	 * Sets the cached filter used by the instigator of the effect.
	 * If set to nullptr, it would be cleared and would use the default settings.
	 * 
	 * @param filter Instigator filter
	 */
	UFUNCTION(BlueprintCallable)
	void SetInstigatorFilter(const URuneFilter* filter);

	/**
	 * Whether or not the actor has been filtered.
	 * 
	 * The fact that something has been filtered out means
	 * that it should be discarded from the flow.
	 * Whether it is applying an effect or checking whether an
	 * effect can be applied to an actor, if it is filtered out,
	 * the given actor should NOT get any effect.
	 *
	 * @param actor Actor to be filtered.
	 * @return If true, actor is filtered (discarded from the flow).
	 */
	bool Filter(const AActor& actor) const;

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
	void SubmitApply(const FRuneEffectPayload& Payload);
	void SubmitRevert(const FRuneEffectPayload& Payload);

	/**
	 * Manages the effect application to the specified AActor.
	 * (e.g. subtract X health points)
	 *
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will receive the effect application.
	 */
	virtual void Apply(const FRuneEffectPayload& Payload);

	/**
	 * Manages the effect "undo" to the specified AActor.
	 * (e.g. recover X health points)
	 *
	 * @param actor Actor which will receive the effect "undo".
	 */
	virtual void Revert(const FRuneEffectPayload& Payload);

	/**
	 * Blueprint version of Filter() method.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Filter"))
	bool InvokeFilter(const AActor* actor) const;

	/**
	 * Intermediate Apply() method used in the filtering process.
	 */
	UFUNCTION()
	virtual FRuneEffectHandle Activate(const FRuneEffectPayload& Payload);

	/**
	 * Intermediate Apply() method used in the filtering process.
	 */
	UFUNCTION()
	void ActivateRaw(const FRuneEffectPayload& Payload);

	/**
	 * Intermediate Revert() method used in the filtering process.
	 * Filtering is not needed. Deactivation implies a previous filtered 
	 * and successfully activation.
	 */
	UFUNCTION()
	virtual void Deactivate(const FRuneEffectHandle& Handle);

public:
	/** Whether the effect should use a custom filter */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings")
	bool overrideFilter;

	/** Filter that overrides the global used filter */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", meta = (EditCondition = "overrideFilter==true", EditConditionHides))
	URuneFilter* customFilter;

	/** Application Mode should be used */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", Instanced, NoClear, meta = (NoResetToDefault))
	class URuneEffectApplicationMode* ApplicationMode;

	/** Delegate invoked when a effect has been applied */
	UPROPERTY(BlueprintAssignable, Category = "RuneEffect: General Settings")
	FEffectApplicationDelegate onEffectApplied;

	/** Delegate invoked when a effect has been reverted */
	UPROPERTY(BlueprintAssignable, Category = "RuneEffect: General Settings")
	FEffectApplicationDelegate onEffectReverted;

protected:
	/**
	 * What faction this effect uses to filter.
	 * Used as a "modifiable constant".
	 * Although it could be done, this should not be modified dynamically.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RuneEffect: General Settings", meta = (Bitmask, BitmaskEnum = ERuneFilterFaction, DisplayAfter = "overrideFilter"))
	uint8 filterFaction;

	/** Cached instigator. It could be nullptr. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RuneEffect: Debug Variables")
	AController* runeInstigator;

	/** Cached instigator RuneFilter. It could be nullptr. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RuneEffect: Debug Variables")
	const URuneFilter* instigatorFilter;

private:
	friend class URuneEffectApplicationMode;
	friend class URuneBaseComponent;
	friend class ARuneTangibleAgent;
	friend class URuneUtils;
};
