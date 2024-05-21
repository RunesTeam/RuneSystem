

#pragma once

#include "CoreMinimal.h"
#include "RuneEffectApplicationMode.generated.h"


/**
 * An Effect Application Mode handles how an effect is activated, deactivated, applies and/or reverted.
 * An effect is always deactivated when an activation has being performed.
 * An effect application or reversion is not mandatory at all (but it would do nothing if neither is called)
 */
UCLASS(Abstract, Blueprintable, EditInlineNew, AutoExpandCategories = "RuneEffectApplicationMode: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API URuneEffectApplicationMode : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URuneEffectApplicationMode() = default;

public:
	/**
	 * Handles how an effect is activated.
	 * Typically it would also handle the application of the effect.
	 * 
	 * @param Handle Valid effect activation handle
	 */
	virtual void HandleEffectActivation(const struct FRuneEffectHandle& Handle);

	/**
	 * Handles how an effect is deactivated.
	 * Typically it would handle effect cancellation, interruption, etc.
	 *
	 * @param Handle Valid effect activation handle
	 */
	virtual void HandleEffectDeactivation(const struct FRuneEffectHandle& Handle);

protected:
	/**
	 * Triggers an activation of an effect given a valid handle.
	 * Whether or not is a new activation or a reset activation is
	 * determined by the application mode itself by redefining
	 * URuneEffectApplicationMode::HandleEffectActivation.
	 * 
	 * @param Handle Valid effect activation handle
	 */
	void SubmitActivation(const struct FRuneEffectHandle& Handle);

	/**
	 * Triggers an deactivation of an effect given a valid handle.
	 *
	 * @param Handle Valid effect activation handle
	 */
	void SubmitDeactivation(const struct FRuneEffectHandle& Handle);

	/**
	 * Triggers an Apply call of an effect given a valid handle.
	 *
	 * @param Handle Valid effect activation handle
	 */
	void SubmitApply(const struct FRuneEffectHandle& Handle);

	/**
	 * Triggers a Revert call of an effect given a valid handle.
	 *
	 * @param Handle Valid effect activation handle
	 */
	void SubmitRevert(const struct FRuneEffectHandle& Handle);

protected:
	/**
	 * Handles how an effect is activated.
	 * Typically it would also handle the application of the effect.
	 *
	 * @param Handle Valid effect activation handle
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "HandleEffectActivation"))
	void ReceiveHandleEffectActivation(const struct FRuneEffectHandle& Handle);

	/**
	 * Handles how an effect is deactivated.
	 *
	 * @param Handle Valid effect activation handle
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "HandleEffectDeactivation"))
	void ReceiveHandleEffectDeactivation(const struct FRuneEffectHandle& Handle);

};
