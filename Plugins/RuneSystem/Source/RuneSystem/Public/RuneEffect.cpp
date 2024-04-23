


#include "RuneEffect.h"
#include "RuneCompatible.h"
#include "RuneFilter.h"
#include "ApplicationType/EoTComponent.h"
#include "ApplicationType/StatusComponent.h"



URuneEffect::URuneEffect() :
	overrideFilter(false),
	customFilter(nullptr),
	applicationType(EApplicationType::IMMEDIATE),
	ticks(5),
	tickRate(0.5f),
	duration(5.0f),
	trimTickDistribution(true),
	filterFaction(static_cast<uint8>(ERuneFilterFaction::FACTION_B)),
	runeInstigator(nullptr),
	instigatorFilter(nullptr)
{
	// do not tick by default
	PrimaryComponentTick.bCanEverTick = false;
}

#if WITH_EDITOR
bool URuneEffect::CanEditChange(const FProperty* InProperty) const
{
	const bool result = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneEffect, filterFaction))
	{
		return result && !HasBegunPlay();
	}

	return result;
}
#endif

void URuneEffect::Apply(AController* instigator, AActor* causer, AActor* target)
{
	// by default calls the blueprint version
	ReceiveApply(instigator, causer, target);
}

void URuneEffect::Revert(AActor* target)
{
	// by default calls the blueprint version
	ReceiveRevert(target);
}

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

bool URuneEffect::InvokeFilter(const AActor* actor) const
{
	if (actor == nullptr)
	{
		return false;
	}
	return Filter(*actor);
}

void URuneEffect::InternalApply(AController* instigator, AActor* causer, AActor* target, FBooleanPtr success)
{
	// if actor is filtered, do NOT apply the effect
	bool filtered = Filter(*target);
	if (success.value != nullptr)
	{
		*success.value |= !filtered;
	}
	if (filtered)
	{
		return;
	}

	switch (applicationType)
	{
	case EApplicationType::IMMEDIATE:
		ApplyEffectInstant(instigator , causer, target);
		break;
	case EApplicationType::OVER_TIME:
		ApplyEffectOverTime(instigator, causer, target);
		break;
	case EApplicationType::STATUS:
		ApplyEffectStatus(instigator, causer, target);
		break;
	default:
		Apply(instigator, causer, target);
		break;
	}
}

void URuneEffect::InternalRevert(AActor* target, FBooleanPtr success)
{
	// filtering check
	bool filtered = Filter(*target);
	if (success.value != nullptr)
	{
		*success.value |= !filtered;
	}
	if (filtered)
	{
		return;
	}

	switch (applicationType)
	{
	case EApplicationType::IMMEDIATE:
		RevertEffectInstant(target);
		break;
	case EApplicationType::OVER_TIME:
		onEffectReverted.Broadcast(target);
		break;
	case EApplicationType::STATUS:
		onEffectReverted.Broadcast(target);
		break;
	default:
		RevertEffectInstant(target);
		break;
	}
}

void URuneEffect::ApplyEffectInstant(AController* instigator, AActor* causer, AActor* target)
{
	Apply(instigator, causer, target);

	// broadcast effect application
	onEffectApplied.Broadcast(target);
}

void URuneEffect::ApplyEffectOverTime(AController* instigator, AActor* causer, AActor* target)
{
	UActorComponent* component = target->AddComponentByClass(UEoTComponent::StaticClass(), false, FTransform::Identity, true);
	UEoTComponent* eotComponent = Cast<UEoTComponent>(component);
	if (eotComponent == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("[RuneEffect] EoTComponent is nullptr"));
		return;
	}
	eotComponent->Configure(this, instigator, ticks, duration, trimTickDistribution, tickRate);
	onEffectReverted.AddDynamic(eotComponent, &UEoTComponent::RemoveFromOwner);
	target->FinishAddComponent(component, false, FTransform::Identity);
}

void URuneEffect::ApplyEffectStatus(AController* instigator, AActor* causer, AActor* target)
{
	UActorComponent* component = target->AddComponentByClass(UStatusComponent::StaticClass(), false, FTransform::Identity, true);
	UStatusComponent* statusComponent = Cast<UStatusComponent>(component);
	if (statusComponent == nullptr)
	{
		UE_LOG(LogTemp, Display, TEXT("[RuneEffect] StatusComponent is nullptr"));
		return;
	}
	statusComponent->Configure(this, instigator, duration);
	target->FinishAddComponent(component, false, FTransform::Identity);
}

void URuneEffect::RevertEffectInstant(AActor* target)
{
	Revert(target);

	// broadcast effect revertion
	onEffectReverted.Broadcast(target);
}
