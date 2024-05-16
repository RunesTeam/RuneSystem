


#include "RuneEffect.h"
#include "RuneSystem.h"
#include "RuneFilter.h"
#include "ApplicationMode/InstantRuneEffectApplicationMode.h"


URuneEffect::URuneEffect() :
	bOverrideFilter(false),
	CustomFilter(nullptr),
	ApplicationMode(nullptr),
	FilterFaction(static_cast<uint8>(ERuneFilterFaction::FACTION_B))
{
	ApplicationMode = CreateDefaultSubobject<UInstantRuneEffectApplicationMode>("Application Mode");
}

#if WITH_EDITOR
bool URuneEffect::CanEditChange(const FProperty* InProperty) const
{
	const bool result = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneEffect, FilterFaction))
	{
		return result;// && !HasBegunPlay();
	}

	return result;
}
#endif

bool URuneEffect::Filter(const FRuneEffectPayload& Payload) const
{
	check(IsValid(Payload.Target) && IsValid(Payload.RuneFilter));

	const URuneFilter* RuneFilter = bOverrideFilter ? CustomFilter : Payload.RuneFilter;
	return !(FilterFaction & ~RuneFilter->Filter(*Payload.Target, GetClass()));
}

void URuneEffect::SubmitApply(const FRuneEffectPayload& Payload)
{
	Apply(Payload);
	OnEffectApplied.Broadcast(Payload);
}

void URuneEffect::SubmitRevert(const FRuneEffectPayload& Payload)
{
	Revert(Payload);
	OnEffectReverted.Broadcast(Payload);
}

void URuneEffect::Apply(const FRuneEffectPayload& Payload)
{
	// by default calls the blueprint version
	ReceiveApply(Payload);
}

void URuneEffect::Revert(const FRuneEffectPayload& Payload)
{
	// by default calls the blueprint version
	ReceiveRevert(Payload);
}

FRuneEffectHandle URuneEffect::Activate(const FRuneEffectPayload& Payload)
{
	// if actor is filtered, do NOT apply the effect
	bool bFiltered = Filter(Payload);
	if (bFiltered)
	{
		return FRuneEffectHandle::Invalid;
	}

	// Create valid handle
	const FRuneEffectHandle& Handle = URuneSystem::CreateEffectHandle(this, Payload);
	ApplicationMode->HandleEffectActivation(Handle);

	return Handle;
}

void URuneEffect::Deactivate(const FRuneEffectHandle& Handle)
{
	check(Handle.Effect == this);
	ApplicationMode->HandleEffectDeactivation(Handle);
	URuneSystem::DisposeEffectHandle(Handle);
}
