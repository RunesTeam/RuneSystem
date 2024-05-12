

#pragma once

#include "CoreMinimal.h"
#include "RuneEffectApplicationMode.h"
#include "InstantRuneEffectApplicationMode.generated.h"


UCLASS(Blueprintable, EditInlineNew, AutoExpandCategories = "InstantRuneEffectApplicationMode: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API UInstantRuneEffectApplicationMode : public URuneEffectApplicationMode
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UInstantRuneEffectApplicationMode();

public:
	virtual void HandleEffectActivation(const FRuneEffectHandle& Handle) override;
	virtual void HandleEffectDeactivation(const FRuneEffectHandle& Handle) override;

};
