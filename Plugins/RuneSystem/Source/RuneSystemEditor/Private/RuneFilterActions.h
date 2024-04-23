// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "AssetTypeActions_Base.h"


class RUNESYSTEMEDITOR_API FRuneFilterActions : public FAssetTypeActions_Base
{
public:
	FRuneFilterActions(EAssetTypeCategories::Type category);
	~FRuneFilterActions();

	virtual FText GetName() const override;
	virtual UClass* GetSupportedClass() const override;
	virtual FColor GetTypeColor() const override;
	virtual uint32 GetCategories() override;
	virtual FText GetAssetDescription(const FAssetData& AssetData) const override;

	virtual bool HasActions(const TArray<UObject*>& InObjects) const override;
	virtual void GetActions(const TArray<UObject*>& InObjects, FMenuBuilder& MenuBuilder) override;

private:
	EAssetTypeCategories::Type _category;
};