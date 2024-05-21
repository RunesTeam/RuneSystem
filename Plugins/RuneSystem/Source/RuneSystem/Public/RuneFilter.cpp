


#include "RuneFilter.h"
#include "Effect/RuneEffect.h"


FRuneFilterData::FRuneFilterData() :
	filterUsage(ERuneFilterUsage::EXCLUSIVE),
	filterType(0),
	factionAActorClassFilter(),
	factionATagsFilter(),
	factionAComponentClassFilter(),
	factionBActorClassFilter(),
	factionBTagsFilter(),
	factionBComponentClassFilter()
{
}

URuneFilter::URuneFilter() :
	runeFilterData(),
	runeFilterDataOverrides()
{
}

#if WITH_EDITOR
bool URuneFilter::CanEditChange(const FProperty* InProperty) const
{
	const bool result = Super::CanEditChange(InProperty);

	/*if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneFilter, factionAActorClassFilter))
	{
		return result && IsFilteredByType(ERuneFilterType::ACTOR_CLASS);
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneFilter, factionATagsFilter))
	{
		return result && IsFilteredByType(ERuneFilterType::TAGS);
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneFilter, factionAComponentClassFilter))
	{
		return result && IsFilteredByType(ERuneFilterType::COMPONENT_CLASS);
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneFilter, factionBActorClassFilter))
	{
		return result && IsFilteredByType(ERuneFilterType::ACTOR_CLASS);
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneFilter, factionBTagsFilter))
	{
		return result && IsFilteredByType(ERuneFilterType::TAGS);
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneFilter, factionBComponentClassFilter))
	{
		return result && IsFilteredByType(ERuneFilterType::COMPONENT_CLASS);
	}*/

	return result;
}
#endif

ERuneFilterUsage URuneFilter::GetFilterUsage(TSubclassOf<URuneEffect> effectClass) const
{
	if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
	{
		return runeFilterDataOverrides[effectClass].filterUsage;
	}

	return runeFilterData.filterUsage;
}

bool URuneFilter::IsFilteredByType(ERuneFilterType type, TSubclassOf<URuneEffect> effectClass) const
{
	if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
	{
		return (runeFilterDataOverrides[effectClass].filterType & static_cast<uint8>(type)) != 0;
	}

	return (runeFilterData.filterType & static_cast<uint8>(type)) != 0;
}

bool URuneFilter::TryGetActorClassFilter(TArray<TSubclassOf<AActor>>& outFilter, ERuneFilterFaction faction, TSubclassOf<URuneEffect> effectClass) const
{
	if (IsFilteredByType(ERuneFilterType::ACTOR_CLASS, effectClass))
	{
		if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
		{
			const TArray<TSubclassOf<AActor>>& ActorClassFilter = faction == ERuneFilterFaction::FACTION_B
				? runeFilterDataOverrides[effectClass].factionBActorClassFilter 
				: runeFilterDataOverrides[effectClass].factionAActorClassFilter;
			outFilter = ActorClassFilter;
			return true;
		}

		const TArray<TSubclassOf<AActor>>& ActorClassFilter = faction == ERuneFilterFaction::FACTION_B 
			? runeFilterData.factionBActorClassFilter 
			: runeFilterData.factionAActorClassFilter;
		outFilter = ActorClassFilter;
		return true;
	}

	return false;
}

bool URuneFilter::TryGetTagsFilter(TArray<FName>& outFilter, ERuneFilterFaction faction, TSubclassOf<URuneEffect> effectClass) const
{
	if (IsFilteredByType(ERuneFilterType::TAGS, effectClass))
	{
		if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
		{
			const TArray<FName>& TagsFilter = faction == ERuneFilterFaction::FACTION_B 
				? runeFilterDataOverrides[effectClass].factionBTagsFilter 
				: runeFilterDataOverrides[effectClass].factionATagsFilter;
			outFilter = TagsFilter;
			return true;
		}

		const TArray<FName>& TagsFilter = faction == ERuneFilterFaction::FACTION_B 
			? runeFilterData.factionBTagsFilter 
			: runeFilterData.factionATagsFilter;
		outFilter = TagsFilter;
		return true;
	}

	return false;
}

bool URuneFilter::TryGetComponentClassFilter(TArray<TSubclassOf<UActorComponent>>& outFilter, ERuneFilterFaction faction, TSubclassOf<URuneEffect> effectClass) const
{
	if (IsFilteredByType(ERuneFilterType::COMPONENT_CLASS, effectClass))
	{
		if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
		{
			const TArray<TSubclassOf<UActorComponent>>& ComponentClassFilter = faction == ERuneFilterFaction::FACTION_B 
				? runeFilterDataOverrides[effectClass].factionBComponentClassFilter 
				: runeFilterDataOverrides[effectClass].factionAComponentClassFilter;
			outFilter = ComponentClassFilter;
			return true;
		}

		const TArray<TSubclassOf<UActorComponent>>& ComponentClassFilter = faction == ERuneFilterFaction::FACTION_B 
			? runeFilterData.factionBComponentClassFilter 
			: runeFilterData.factionAComponentClassFilter;
		outFilter = ComponentClassFilter;
		return true;
	}

	return false;
}

uint8 URuneFilter::Filter(const AActor& actor, TSubclassOf<URuneEffect> effectClass) const
{
	uint8 filtered = static_cast<uint8>(ERuneFilterFaction::NONE);
	for (ERuneFilterType type : TEnumRange<ERuneFilterType>())
	{
		if (!IsFilteredByType(type, effectClass)) continue;

		switch (type)
		{
		case ERuneFilterType::ACTOR_CLASS:
			filtered |= FilterByActorClass(actor, effectClass);
			break;
		case ERuneFilterType::TAGS:
			filtered |= FilterByTags(actor, effectClass);
			break;
		case ERuneFilterType::COMPONENT_CLASS:
			filtered |= FilterByComponentClass(actor, effectClass);
			break;
		}
	}

	return filtered;
}

uint8 URuneFilter::FilterByActorClass(const AActor& actor, TSubclassOf<URuneEffect> effectClass) const
{
	if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
	{
		return FilterByActorClassByFilterData(actor, runeFilterDataOverrides[effectClass]);
	}

	return FilterByActorClassByFilterData(actor, runeFilterData);
}

uint8 URuneFilter::FilterByActorClassByFilterData(const AActor& actor, const FRuneFilterData& filterData) const
{
	uint8 factionMask = static_cast<uint8>(ERuneFilterFaction::NONE);

	// faction A
	{
		const TArray<TSubclassOf<AActor>>& ActorClassFilter = filterData.factionAActorClassFilter;
		bool result =
			ActorClassFilter.ContainsByPredicate(
				[&actor](const TSubclassOf<AActor>& actorClass) -> bool
				{
					return actor.IsA(actorClass);
				}
		);
		factionMask |= (static_cast<uint8>(ERuneFilterFaction::FACTION_A) * result);
	}

	// faction B
	{
		const TArray<TSubclassOf<AActor>>& ActorClassFilter = filterData.factionBActorClassFilter;
		bool result =
			ActorClassFilter.ContainsByPredicate(
				[&actor](const TSubclassOf<AActor>& actorClass) -> bool
				{
					return actor.IsA(actorClass);
				}
		);
		factionMask |= (static_cast<uint8>(ERuneFilterFaction::FACTION_B) * result);
	}

	return filterData.filterUsage == ERuneFilterUsage::EXCLUSIVE ?
		factionMask :
		~factionMask & static_cast<uint8>(ERuneFilterFaction::ALL);
}

uint8 URuneFilter::FilterByTags(const AActor& actor, TSubclassOf<URuneEffect> effectClass) const
{
	if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
	{
		return FilterByTagsByFilterData(actor, runeFilterDataOverrides[effectClass]);
	}

	return FilterByTagsByFilterData(actor, runeFilterData);
}

uint8 URuneFilter::FilterByTagsByFilterData(const AActor& actor, const FRuneFilterData& filterData) const
{
	uint8 factionMask = static_cast<uint8>(ERuneFilterFaction::NONE);

	// allies
	{
		const TArray<FName>& TagsFilter = filterData.factionATagsFilter;
		bool result =
			TagsFilter.ContainsByPredicate(
				[&actor](const FName& name) -> bool
				{
					return actor.Tags.Contains(name);
				}
		);
		factionMask |= (static_cast<uint8>(ERuneFilterFaction::FACTION_A) * result);
	}

	// enemies
	{
		const TArray<FName>& TagsFilter = filterData.factionBTagsFilter;
		bool result =
			TagsFilter.ContainsByPredicate(
				[&actor](const FName& name) -> bool
				{
					return actor.Tags.Contains(name);
				}
		);
		factionMask |= (static_cast<uint8>(ERuneFilterFaction::FACTION_B) * result);
	}

	return filterData.filterUsage == ERuneFilterUsage::EXCLUSIVE ?
		factionMask :
		~factionMask & static_cast<uint8>(ERuneFilterFaction::ALL);
}

uint8 URuneFilter::FilterByComponentClass(const AActor& actor, TSubclassOf<URuneEffect> effectClass) const
{
	if (effectClass != nullptr && runeFilterDataOverrides.Contains(effectClass))
	{
		return FilterByComponentClassByFilterData(actor, runeFilterDataOverrides[effectClass]);
	}

	return FilterByComponentClassByFilterData(actor, runeFilterData);
}

uint8 URuneFilter::FilterByComponentClassByFilterData(const AActor& actor, const FRuneFilterData& filterData) const
{
	uint8 factionMask = static_cast<uint8>(ERuneFilterFaction::NONE);

	// faction A
	{
		const TArray<TSubclassOf<UActorComponent>>& ComponentClassFilter = filterData.factionAComponentClassFilter;
		bool result =
			ComponentClassFilter.ContainsByPredicate(
				[&actor](const TSubclassOf<UActorComponent>& componentClass) -> bool
				{
					return actor.GetComponentByClass(componentClass) != nullptr;
				}
		);
		factionMask |= (static_cast<uint8>(ERuneFilterFaction::FACTION_A) * result);
	}

	// faction B
	{
		const TArray<TSubclassOf<UActorComponent>>& ComponentClassFilter = filterData.factionBComponentClassFilter;
		bool result =
			ComponentClassFilter.ContainsByPredicate(
				[&actor](const TSubclassOf<UActorComponent>& componentClass) -> bool
				{
					return actor.GetComponentByClass(componentClass) != nullptr;
				}
		);
		factionMask |= (static_cast<uint8>(ERuneFilterFaction::FACTION_B) * result);
	}

	return filterData.filterUsage == ERuneFilterUsage::EXCLUSIVE ?
		factionMask :
		~factionMask & static_cast<uint8>(ERuneFilterFaction::ALL);
}
