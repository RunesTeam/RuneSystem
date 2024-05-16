


#include "RuneUtils.h"
#include "RuneEffect.h"
#include "RuneEffectApplicationMode.h"
#include "RuneEffectHandle.h"


URuneEffect* URuneUtils::CreateEffectInstance(const TSubclassOf<class URuneEffect> EffectClass, class URuneEffectApplicationMode*& OutApplicationModeInstance, bool bOverrideApplicationMode, const TSubclassOf<class URuneEffectApplicationMode> ApplicationModeClass)
{
	if (!IsValid(EffectClass))
	{
		return nullptr;
	}

	URuneEffect* OutEffect = NewObject<URuneEffect>(GetTransientPackage(), EffectClass, TEXT("Rune Effect"), RF_Transient);
	if (IsValid(OutEffect) && bOverrideApplicationMode)
	{
		OutEffect->ApplicationMode = NewObject<URuneEffectApplicationMode>(GetTransientPackage(), ApplicationModeClass, TEXT("Rune Effect Application Mode"), RF_Transient);
		check(IsValid(OutEffect->ApplicationMode));
	}

	return OutEffect;
}

FRuneEffectHandle URuneUtils::ActivateEffectByClass(const TSubclassOf<URuneEffect> EffectClass, const FRuneEffectPayload& Payload, URuneEffect*& OutEffect)
{
	if (!IsValid(EffectClass) || !IsValid(Payload.Target))
	{
		return FRuneEffectHandle::Invalid;
	}

	OutEffect = NewObject<URuneEffect>(GetTransientPackage(), EffectClass, TEXT("Rune Effect"), RF_Transient);
	check(IsValid(OutEffect));

	return OutEffect->Activate(Payload);
}

FRuneEffectHandle URuneUtils::ActivateEffect(URuneEffect* Effect, const FRuneEffectPayload& Payload)
{
	if (!IsValid(Effect) || !IsValid(Payload.Target))
	{
		return FRuneEffectHandle::Invalid;
	}

	return Effect->Activate(Payload);
}

void URuneUtils::DeactivateEffect(const FRuneEffectHandle& Handle)
{
	if (!IsValid(Handle.Effect) || !IsValid(Handle.Payload.Target))
	{
		return;
	}

	Handle.Effect->Deactivate(Handle);
}
