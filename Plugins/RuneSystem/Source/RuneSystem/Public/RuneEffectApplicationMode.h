

#pragma once

#include "CoreMinimal.h"
#include "RuneEffectPayload.h"
#include "RuneEffectApplicationMode.generated.h"


UCLASS(Abstract, Blueprintable, EditInlineNew, AutoExpandCategories = "RuneEffectApplicationMode: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API URuneEffectApplicationMode : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URuneEffectApplicationMode() = default;

public:
	virtual void HandleEffectActivation(const FRuneEffectHandle& Handle);
	virtual void HandleEffectDeactivation(const FRuneEffectHandle& Handle);

protected:
	// TODO: document
	void SubmitApply(const FRuneEffectHandle& Handle);
	void SubmitRevert(const FRuneEffectHandle& Handle);

	// triggers an activation
	void SubmitActivation(const FRuneEffectHandle& Handle);
	// triggers a deactivation
	void SubmitDeactivation(const FRuneEffectHandle& Handle);

protected:
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "HandleEffectActivation"))
	void ReceiveHandleEffectActivation(const FRuneEffectHandle& Handle);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "HandleEffectDeactivation"))
	void ReceiveHandleEffectDeactivation(const FRuneEffectHandle& Handle);

};
