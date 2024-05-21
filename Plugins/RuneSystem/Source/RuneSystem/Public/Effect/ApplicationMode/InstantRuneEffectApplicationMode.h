

#pragma once

#include "CoreMinimal.h"
#include "RuneEffectApplicationMode.h"
#include "InstantRuneEffectApplicationMode.generated.h"


/**
 * Instant Application Mode activates and applies the effect over a given target immediately.
 * Deactivation happens right after the application of the effect.
 */
UCLASS(Blueprintable, EditInlineNew, AutoExpandCategories = "InstantRuneEffectApplicationMode: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking))
class RUNESYSTEM_API UInstantRuneEffectApplicationMode : public URuneEffectApplicationMode
{
	GENERATED_BODY()

public:
	// Sets default values for this object's properties
	UInstantRuneEffectApplicationMode();

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

};
