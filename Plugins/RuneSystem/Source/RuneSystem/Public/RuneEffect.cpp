


#include "RuneEffect.h"
#include "RuneCompatible.h"
#include "RuneFilter.h"
#include "ApplicationMode/InstantRuneEffectApplicationMode.h"
#include "RuneSystem.h"


URuneEffect::URuneEffect() :
	overrideFilter(false),
	customFilter(nullptr),
	ApplicationMode(nullptr),
	filterFaction(static_cast<uint8>(ERuneFilterFaction::FACTION_B)),
	runeInstigator(nullptr),
	instigatorFilter(nullptr)
{
	ApplicationMode = CreateDefaultSubobject<UInstantRuneEffectApplicationMode>("Application Mode");
}

#if WITH_EDITOR
bool URuneEffect::CanEditChange(const FProperty* InProperty) const
{
	const bool result = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneEffect, filterFaction))
	{
		return result;// && !HasBegunPlay();
	}

	return result;
}
#endif

const URuneFilter* URuneEffect::GetUsedFilter() const
{
	if (!overrideFilter)
	{
		return instigatorFilter;
	}
	return customFilter;
}

AController* URuneEffect::GetInstigator() const
{
	return runeInstigator;
}

void URuneEffect::SetInstigator(AController* instigator)
{
	runeInstigator = instigator;
}

const URuneFilter* URuneEffect::GetInstigatorFilter() const
{
	return instigatorFilter;
}

void URuneEffect::SetInstigatorFilter(const URuneFilter* filter)
{
	instigatorFilter = filter;
}

bool URuneEffect::Filter(const AActor& actor) const
{
	const URuneFilter* runeFilter = GetUsedFilter();
	if (runeFilter == nullptr)
	{
		return false;
	}

	return !(filterFaction & ~runeFilter->Filter(actor, GetClass()));
}

void URuneEffect::SubmitApply(const FRuneEffectPayload& Payload)
{
	Apply(Payload);
	onEffectApplied.Broadcast(Payload);
}

void URuneEffect::SubmitRevert(const FRuneEffectPayload& Payload)
{
	Revert(Payload);
	onEffectReverted.Broadcast(Payload);
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

bool URuneEffect::InvokeFilter(const AActor* actor) const
{
	if (actor == nullptr)
	{
		return false;
	}
	return Filter(*actor);
}

FRuneEffectHandle URuneEffect::Activate(const FRuneEffectPayload& Payload)
{
	// if actor is filtered, do NOT apply the effect
	bool bFiltered = Filter(*Payload.Target);
	if (bFiltered)
	{
		return FRuneEffectHandle::Invalid;
	}

	// Create valid handle
	const FRuneEffectHandle& Handle = URuneSystem::CreateEffectHandle(this, Payload);
	ApplicationMode->HandleEffectActivation(Handle);

	return Handle;
}

void URuneEffect::ActivateRaw(const FRuneEffectPayload& Payload)
{
	Activate(Payload);
}

void URuneEffect::Deactivate(const FRuneEffectHandle& Handle)
{
	check(Handle.Effect == this);
	ApplicationMode->HandleEffectDeactivation(Handle);
	URuneSystem::DisposeEffectHandle(Handle);
}
