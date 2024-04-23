

#pragma once

#include "CoreMinimal.h"
#include "Misc/EnumRange.h"
#include "UObject/NoExportTypes.h"
#include "RuneFilter.generated.h"

UENUM(BlueprintType)
enum class ERuneFilterUsage : uint8
{
	EXCLUSIVE		= 0			UMETA(Tooltip = "Effects will not be applied if filtered"),
	INCLUSIVE		= 1			UMETA(Tooltip = "Effects will only be applied if filtered")
};

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ERuneFilterFaction : uint8
{
	NONE			= 0			UMETA(Hidden),
	FACTION_A		= 1 << 0	UMETA(DisplayName = "ALLIES"),
	FACTION_B		= 1 << 1	UMETA(DisplayName = "ENEMIES"),

	ALL				= 3			UMETA(Hidden)
};
ENUM_CLASS_FLAGS(ERuneFilterFaction);

UENUM(BlueprintType, meta = (Bitflags, UseEnumValuesAsMaskValuesInEditor = "true"))
enum class ERuneFilterType : uint8
{
	NONE			= 0			UMETA(Hidden),
	ACTOR_CLASS		= 1 << 0,
	TAGS			= 1 << 1,
	COMPONENT_CLASS = 1 << 2
};
ENUM_CLASS_FLAGS(ERuneFilterType);
ENUM_RANGE_BY_FIRST_AND_LAST(ERuneFilterType, ERuneFilterType::ACTOR_CLASS, ERuneFilterType::TAGS);


USTRUCT()
struct FRuneFilterData
{
	GENERATED_BODY()
	
public:
	FRuneFilterData();
	
public:
	/** Whether filter is used as EXCLUSIVE or INCLUSIVE */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: General Settings")
	ERuneFilterUsage filterUsage;

	/** What predefined filters will be used */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: General Settings", meta = (Bitmask, BitmaskEnum = ERuneFilterType))
	uint8 filterType;

	/** List of the AActors used for filtering */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: Allies Settings", DisplayName = "Allies Actor Class Filter")
	TArray<TSubclassOf<AActor>> factionAActorClassFilter;

	/** List of tags used for filtering */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: Allies Settings", DisplayName = "Allies Tags Filter")
	TArray<FName> factionATagsFilter;

	/** List of the Components used for filtering */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: Allies Settings", DisplayName = "Allies Component Class Filter")
	TArray<TSubclassOf<UActorComponent>> factionAComponentClassFilter;

	/** List of the AActors used for filtering */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: Enemies Settings", DisplayName = "Enemies Actor Class Filter")
	TArray<TSubclassOf<AActor>> factionBActorClassFilter;

	/** List of tags used for filtering */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: Enemies Settings", DisplayName = "Enemies Tags Filter")
	TArray<FName> factionBTagsFilter;

	/** List of the Components used for filtering */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: Enemies Settings", DisplayName = "Enemies Component Class Filter")
	TArray<TSubclassOf<UActorComponent>> factionBComponentClassFilter;

};


/**
 * 
 */
UCLASS(BlueprintType, EditInlineNew)
class RUNESYSTEM_API URuneFilter : public UObject
{
	GENERATED_BODY()
	
public:
	// Sets default values for this object's properties
	URuneFilter();

#if WITH_EDITOR
	/**
	 * Called by the editor to query whether a property of this object is allowed to be modified.
	 * The property editor uses this to disable controls for properties that should not be changed.
	 * When overriding this function you should always call the parent implementation first.
	 *
	 * @param InProperty The property to query
	 *
	 * @return true if the property can be modified in the editor, otherwise false
	 */
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

public:
	/**
	 * Gets the filter usage.
	 * 
	 * @param effectClass Effect class to be filtered
	 * @return Filter usage.
	 */
	ERuneFilterUsage GetFilterUsage(TSubclassOf<class URuneEffect> effectClass = nullptr) const;

	/**
	 * Check filter configuration by ERuneFilterType.
	 * 
	 * @param type ERuneFilterType used to check if its filtered by that type
	 * @param effectClass Effect class to be filtered
	 * @return If true, type ERuneFilterType is used to filter
	 */
	bool IsFilteredByType(ERuneFilterType type, TSubclassOf<class URuneEffect> effectClass = nullptr) const;

	/**
	 * Get array filter used to filter by ActorClass-s.
	 * 
	 * @param outFilter Filter by ActorClass-s
	 * @param faction Faction related to the filter.
	 * @param effectClass Effect class to be filtered
	 * @return If true, filter is used in the filtering process
	 */
	bool TryGetActorClassFilter(TArray<TSubclassOf<AActor>>& outFilter, ERuneFilterFaction faction, TSubclassOf<class URuneEffect> effectClass = nullptr) const;

	/**
	 * Gets the array filter to filter by Tags.
	 * 
	 * @param outFilter Filter by Tags
	 * @param faction Faction related to the filter.
	 * @param effectClass Effect class to be filtered
	 * @return If true, filter is used in the filtering process
	 */
	bool TryGetTagsFilter(TArray<FName>& outFilter, ERuneFilterFaction faction, TSubclassOf<class URuneEffect> effectClass = nullptr) const;

	/**
	 * Get array filter used to filter by ComponentClass-s.
	 *
	 * @param outFilter Filter by ComponentClass-s
	 * @param faction Faction related to the filter.
	 * @param effectClass Effect class to be filtered
	 * @return If true, filter is used in the filtering process
	 */
	bool TryGetComponentClassFilter(TArray<TSubclassOf<UActorComponent>>& outFilter, ERuneFilterFaction faction, TSubclassOf<class URuneEffect> effectClass = nullptr) const;

	/**
	 * Whether or not the actor has been filtered per faction.
	 * The fact that something has been filtered out means
	 * that it should be discarded from the flow.
	 * Whether it is applying an effect or checking whether an
	 * effect can be applied to an actor, if it is filtered out,
	 * the given actor should NOT get any effect.
	 *
	 * @param actor Actor to be filtered.
	 * @param effectClass Effect class to be filtered
	 * @return Faction bitmask.
	 */
	uint8 Filter(const AActor& actor, TSubclassOf<class URuneEffect> effectClass = nullptr) const;

private:
	/**
	 * Helper method used to filter only by ActorClass.
	 *
	 * @param actor Actor to be filtered.
	 * @param effectClass Effect class to be filtered
	 * @return Faction bitmask where the filter was successfull.
	 */
	uint8 FilterByActorClass(const AActor& actor, TSubclassOf<class URuneEffect> effectClass) const;

	/**
	 * Helper method used to filter only by ActorClass by a given filter data.
	 *
	 * @param actor Actor to be filtered.
	 * @param filterData Filter data used to filter.
	 * @param effectClass Effect class to be filtered
	 * @return Faction bitmask where the filter was successfull.
	 */
	uint8 FilterByActorClassByFilterData(const AActor& actor, const FRuneFilterData& filterData) const;

	/**
	 * Helper method used to filter only by Tags.
	 *
	 * @param actor Actor to be filtered.
	 * @param effectClass Effect class to be filtered
	 * @return Faction bitmask where the filter was successfull.
	 */
	uint8 FilterByTags(const AActor& actor, TSubclassOf<class URuneEffect> effectClass) const;

	/**
	 * Helper method used to filter only by Tags by a given filter data.
	 *
	 * @param actor Actor to be filtered.
	 * @param filterData Filter data used to filter.
	 * @return Faction bitmask where the filter was successfull.
	 */
	uint8 FilterByTagsByFilterData(const AActor& actor, const FRuneFilterData& filterData) const;

	/**
	 * Helper method used to filter only by ComponentClass.
	 *
	 * @param actor Actor to be filtered.
	 * @param effectClass Effect class to be filtered
	 * @return Faction bitmask where the filter was successfull.
	 */
	uint8 FilterByComponentClass(const AActor& actor, TSubclassOf<class URuneEffect> effectClass) const;

	/**
	 * Helper method used to filter only by ComponentClass by a given filter data.
	 *
	 * @param actor Actor to be filtered.
	 * @param filterData Filter data used to filter.
	 * @return Faction bitmask where the filter was successfull.
	 */
	uint8 FilterByComponentClassByFilterData(const AActor& actor, const FRuneFilterData& filterData) const;

protected:
	/** Default filtering data */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: General Settings", meta = (ShowOnlyInnerProperties))
	FRuneFilterData runeFilterData;

	/** Filtering data per effect that overrides the default data */
	UPROPERTY(EditAnywhere, Category = "RuneFilter: Advanced Settings", meta = (ForceInlineRow))
	TMap<TSubclassOf<class URuneEffect>, FRuneFilterData> runeFilterDataOverrides;

};
