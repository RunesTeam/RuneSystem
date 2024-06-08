

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RuneBehaviour.h"
#include "RuneEffect.h"
#include "RuneBaseComponent.generated.h"

class IRuneCompatible;
class URuneInternalScheduler;
class URuneTask;

USTRUCT(Blueprintable, BlueprintType)
struct FRuneBehaviourWithEffects
{
	GENERATED_BODY()
	/** Casted rune behaviour which will apply effects */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneBase: General Settings", Instanced, meta = (FullyExpand = true))
	URuneBehaviour* runeBehaviour;

	/** Effects applied by the behaviour */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneBase: General Settings", Instanced, meta = (FullyExpand = true))
	TArray<URuneEffect*> runeEffects;

	bool isValid() const
	{
		if (runeBehaviour == nullptr)
		{
			return false;
		}
		if (runeEffects.Num() <= 0)
		{
			return false;
		}
		for (URuneEffect* runeEffect : runeEffects)
		{
			if (runeEffect == nullptr)
			{
				return false;
			}
		}
		return true;
	}

	void Destroy() {
		if (runeBehaviour != nullptr)
		{
			runeBehaviour->DestroyComponent();
		}
		for (URuneEffect* runeEffect : runeEffects)
		{
			if (runeEffect != nullptr)
			{
				runeEffect->DestroyComponent();
			}
		}
	}
};

USTRUCT(Blueprintable, BlueprintType)
struct FRuneConfiguration
{
public:
	GENERATED_BODY()

	/** Rune cast state machine used to cast the rune behaviour(s) */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneBase: General Settings", Instanced, meta = (FullyExpand = true), BlueprintReadOnly)
	class URuneCastStateMachine* runeCastStateMachine;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneBase: General Settings", EditFixedSize, meta = (ShowOnlyInnerProperties))
	TArray<FRuneBehaviourWithEffects> runeBehavioursWithEffects;

	bool isValid() const
	{
		bool isValid = true;
		for (const FRuneBehaviourWithEffects& rb : runeBehavioursWithEffects)
		{
			isValid &= rb.isValid();
		}

		return runeCastStateMachine != nullptr && isValid;
	}
};


UCLASS(ClassGroup = "RuneSystem", EditInlineNew, AutoExpandCategories = ("RuneBase: General Settings"), Blueprintable, meta = (BlueprintSpawnableComponent)/*, HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking, Replication, Navigation, "Components|Activation")*/)
class RUNESYSTEM_API URuneBaseComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URuneBaseComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;
	/**
	 * Sets cast state machine internal variable to pressed,
	 * potentially altering state
	 */
	UFUNCTION(BlueprintCallable)
	void Press();
	/**
	 * Sets cast state machine internal variable to not pressed,
	 * potentially altering state.
	 */
	UFUNCTION(BlueprintCallable)
	void Release();

	/**
	 * Checks for the integrity of the rune.
	 *
	 * @return If true, the rune is valid
	 */
	UFUNCTION(BlueprintCallable)
	bool IsValid() const;

	/**
	 * Configures the owner that controls the current rune.
	 *
	 * @param owner Controller owner.
	 */
	void SetOwner(IRuneCompatible* owner);

private:
	/**
	 * Configures communication between the different
	 * components that make up a rune.
	 */
	void Configure() const;

	/**
	 * Validates the integrity of the components and
	 * that it is possible to form the rune.
	 *
	 * @return If true, validation ended succesfully
	 */
	bool Validate() const;

public:

	/**
	 * Rune configurations, each one including a rune cast and one or more rune behaviours with rune effects.
	 * When having more than one rune configuration rune tasks are needed to control execution flow.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneBase: General Settings", meta = (ShowOnlyInnerProperties))
	TArray<FRuneConfiguration> runeConfigurations;

	/**
	 * Internal scheduler, responsible for determining which rune configuration should be ticked each frame.
	 * If the rune only has one configuration this component is not needed and can be null.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneBase: Advanced Settings", Instanced, meta = (FullyExpand = true), BlueprintReadOnly)
	URuneInternalScheduler* runeInternalScheduler;

	/**
	 * Rune Tasks are objects that can add an evaluable state to their specific linked rune configuration.
	 * This state is used by the internal scheduler for determining which rune configuration should be ticked
	 * each frame.
	 */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneBase: Advanced Settings", Instanced, EditFixedSize, meta = (ShowOnlyInnerProperties, EditCondition = "runeInternalScheduler!=nullptr", EditConditionHides))
	TArray<URuneTask*> runeTasks;


protected:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
	virtual void PostEditChangeChainProperty(struct FPropertyChangedChainEvent& PropertyChangedChainEvent) override;
#endif
};
