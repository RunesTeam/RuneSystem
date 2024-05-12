

#pragma once

#include "CoreMinimal.h"
#include "RuneEffectApplicationMode.h"
#include "StatusRuneEffectApplicationMode.generated.h"


UCLASS(Blueprintable, EditInlineNew, AutoExpandCategories = "StatusRuneEffectApplicationMode: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API UStatusRuneEffectApplicationMode : public URuneEffectApplicationMode
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UStatusRuneEffectApplicationMode();

public:
	virtual void HandleEffectActivation(const FRuneEffectHandle& Handle) override;
	virtual void HandleEffectDeactivation(const FRuneEffectHandle& Handle) override;

public:
	/**
	 * How much time - in seconds - should the effect last.
	 * If negative it will last until reversion.
	 */
	UPROPERTY(EditAnywhere, Category = "StatusRuneEffectApplicationMode: General Settings")
	float Duration;

};


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