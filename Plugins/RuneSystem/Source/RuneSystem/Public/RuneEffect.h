

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utils/RuneUtils.h"
#include "RuneEffect.generated.h"


class AActor;
class URuneFilter;


DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FEffectApplicationDelegate, class AActor*, actor);

UENUM()
enum class EApplicationType
{
	IMMEDIATE = 0,
	OVER_TIME,
	STATUS,
};

UCLASS(Abstract, Blueprintable, EditInlineNew, AutoExpandCategories = "RuneEffect: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API URuneEffect : public UActorComponent
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
	 * Manages the effect application to the specified AActor.
	 * (e.g. substract X health points)
	 * 
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will recieve the effect application.
	 */
	virtual void Apply(AController* instigator, AActor* causer, AActor* target);

	/**
	 * Manages the effect "undo" to the specified AActor.
	 * (e.g. recover X health points)
	 *
	 * @param actor Actor which will recieve the effect "undo".
	 */
	virtual void Revert(AActor* actor);

	/**
	 * Gets the filter that it is going to be used for filtering.
	 * If OverrideFilter is true, CustomFilter will be returned.
	 *
	 * @return Used filter, either it is CustomFilter or the global filter.
	 */
	UFUNCTION(BlueprintCallable)
	const URuneFilter* GetUsedFilter() const;

	/**
	 * Get the chached instigator of the effect.
	 * It could be nullptr.
	 * 
	 * @return Instigator.
	 */
	UFUNCTION(BlueprintCallable)
	AController* GetInstigator() const;

	/**
	 * Sets the cached intigator of the effect.
	 * 
	 * @param filter Instigator filter
	 */
	UFUNCTION(BlueprintCallable)
	void SetInstigator(AController* instigator);

	/**
	 * Get the chached filter used by the instigator of the effect.
	 * It could be nullptr.
	 * 
	 * @return Instigator filter.
	 */
	UFUNCTION(BlueprintCallable)
	const URuneFilter* GetInstigatorFilter() const;

	/**
	 * Sets the cached filter used by the intigator of the effect.
	 * If set to nullptr, it would be cleared and would use the default settings.
	 * 
	 * @param filter Instigator filter
	 */
	UFUNCTION(BlueprintCallable)
	void SetInstigatorFilter(const URuneFilter* filter);

	/**
	 * Wheter or not the actor has been filtered.
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
	 * (e.g. substract X health points)
	 *
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will recieve the effect application.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Apply"))
	void ReceiveApply(AController* instigator, AActor* causer, AActor* target);

	/**
	 * Manages the effect "undo" to the specified AActor.
	 * (e.g. recover X health points)
	 *
	 * @param target Actor which will recieve the effect "undo".
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Revert"))
	void ReceiveRevert(AActor* target);

private:
	/**
	 * Blueprint version of Filter() method.
	 */
	UFUNCTION(BlueprintCallable, meta = (DisplayName = "Filter"))
	bool InvokeFilter(const AActor* actor) const;

	/**
	 * Intermediate Apply() method used in the filtering process.
	 */
	UFUNCTION()
	virtual void InternalApply(AController* instigator, AActor* causer, AActor* target, FBooleanPtr success);

	/**
	 * Intermediate Revert() method used in the filtering process.
	 */
	UFUNCTION()
	virtual void InternalRevert(AActor* target, FBooleanPtr success);

	/**
	 * Apply() wrapper.
	 * Added for consistency with the other EApplicationTypes
	 *
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will recieve the effect application.
	 */
	void ApplyEffectInstant(AController* instigator, AActor* causer, AActor* target);

	/**
	 * Delegates the Apply() and Revert() functionality
	 * to an Effect over Time component (EoTComponent).
	 *
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will recieve the effect application.
	 */
	void ApplyEffectOverTime(AController* instigator, AActor* causer, AActor* target);

	/**
	 * Delegates the Apply() and Revert() functionality
	 * to an Status component (StatusComponent).
	 *
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will recieve the effect application.
	 */
	void ApplyEffectStatus(AController* instigator, AActor* causer, AActor* target);

	/**
	 * Revert() wrapper.
	 * Added for consistency with its counterpart (apply)
	 *
	 * @param target Actor which will recieve the effect application.
	 */
	void RevertEffectInstant(AActor* target);

public:
	/** Whether the effect should use a custom filter */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings")
	bool overrideFilter;

	/** Filter that overrides the global used filter */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", meta = (EditCondition = "overrideFilter==true", EditConditionHides))
	URuneFilter* customFilter;

	/** Type of application should be used */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings")
	EApplicationType applicationType;

	/**
	 * How many times should the effect be applied.
	 * Only compatible with effects with positive duration.
	 */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", meta = (EditCondition = "applicationType==EApplicationType::OVER_TIME && duration>0", EditConditionHides))
	uint32 ticks;

	/**
	 * Time window - in seconds - between consecutive ticks.
	 * Necessary for effects with negative (undefined) duration.
	 */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", meta = (EditCondition = "applicationType==EApplicationType::OVER_TIME && duration<0", EditConditionHides))
	float tickRate;
	
	/**
	 * How much time - in seconds - should the effect last.
	 * If negative it will last until revertion.
	 */
	UPROPERTY(BlueprintReadOnly, EditAnywhere, Category = "RuneEffect: General Settings", meta = (EditCondition = "applicationType!=EApplicationType::IMMEDIATE", EditConditionHides))
	float duration;

	/**
	 * Whether the ticks should be adjusted to the duration start-end interval or just distributed in time.
	 * - If true:	|__|__|__|__|
	 * - If false:	_|_|_|_|_|_
	 */
	UPROPERTY(EditAnywhere, Category = "RuneEffect: General Settings", meta = (EditCondition = "applicationType==EApplicationType::OVER_TIME", EditConditionHides))
	bool trimTickDistribution;
	
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
	friend class UEoTComponent;
	friend class UStatusComponent;
	friend class URuneBaseComponent;
	friend class ARuneTangibleAgent;
	friend class URuneUtils;
};
