#include "RuneFilterActions.h"
#include "RuneFilter.h"


FRuneFilterActions::FRuneFilterActions(EAssetTypeCategories::Type category) :
	_category(category)
{

}

FRuneFilterActions::~FRuneFilterActions()
{
}

FText FRuneFilterActions::GetName() const
{
	return FText::FromString(TEXT("RuneFilter"));
}

UClass* FRuneFilterActions::GetSupportedClass() const
{
	return URuneFilter::StaticClass();
}

FColor FRuneFilterActions::GetTypeColor() const
{
	return FColor::Emerald;
}

uint32 FRuneFilterActions::GetCategories()
{
	return _category;
}

FText FRuneFilterActions::GetAssetDescription(const FAssetData& AssetData) const
{
	return FText::FromString(TEXT("RuneFilter is filter that defines what AActors should be affected by the effects"));
}

bool FRuneFilterActions::HasActions(const TArray<UObject*>& InObjects) const
{
	return false;
}

void FRuneFilterActions::GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder)
{
	FAssetTypeActions_Base::GetActions(InObjects, MenuBuilder);
}
