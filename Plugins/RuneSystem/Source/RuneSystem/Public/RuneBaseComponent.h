

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RuneBehaviour.h"
#include "Effect/RuneEffect.h"
#include "RuneBaseComponent.generated.h"

class IRuneCompatible;
//class URuneBehaviour;
//class URuneEffect;

USTRUCT(Blueprintable, BlueprintType)
struct FRuneBehaviourWithEffects
{
	GENERATED_BODY()
	/** Casted rune behaviour which will apply effects */
	UPROPERTY(EditAnywhere, Category = "RuneBase: General Settings", Instanced, meta = (FullyExpand = true))
	URuneBehaviour* runeBehaviour;

	/** Effects applied by the behaviour */
	UPROPERTY(EditAnywhere, Category = "RuneBase: General Settings", Instanced, meta = (FullyExpand = true))
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
				//runeEffect->DestroyComponent();
			}
		}
	}
};

UCLASS(ClassGroup = "RuneSystem", EditInlineNew, AutoExpandCategories = ("RuneBase: General Settings"), Blueprintable, meta = (BlueprintSpawnableComponent), HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking, "Components|Activation"))
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
	/** Rune cast state machine used to cast the rune behaviour */
	UPROPERTY(EditAnywhere, Category = "RuneBase: General Settings", Instanced, meta = (FullyExpand = true), BlueprintReadOnly)
	class URuneCastStateMachine* runeCastStateMachine;

	UPROPERTY(EditAnywhere, Category = "RuneBase: General Settings", EditFixedSize, meta = (ShowOnlyInnerProperties))
	TArray<FRuneBehaviourWithEffects> runeBehavioursWithEffects;

	UPROPERTY(EditAnywhere, Category = "RuneBase: General Settings", Instanced, meta = (FullyExpand = true), BlueprintReadOnly)
	class UObject* test;

protected:

#if WITH_EDITOR
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif
};
