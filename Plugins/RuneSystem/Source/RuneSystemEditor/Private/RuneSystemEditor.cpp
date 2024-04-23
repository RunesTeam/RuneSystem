// Fill out your copyright notice in the Description page of Project Settings.

#include "RuneSystemEditor.h"
#include "Modules/ModuleManager.h"
#include "PropertyEditorModule.h"

#include "Utils/RuneTypes.h"
#include "RuneFilterActions.h"
#include "RuneTangibleAgentTemplateCustomization.h"


IMPLEMENT_MODULE(FRuneSystemEditor, RuneSystemEditor);

void FRuneSystemEditor::StartupModule()
{
	UE_LOG(LogTemp, Warning, TEXT("RuneSystemEditor module startup"));

	// AssetTools module
	{
		FAssetToolsModule& assetsModule = FModuleManager::LoadModuleChecked<FAssetToolsModule>("AssetTools");
		IAssetTools& assetTools = assetsModule.Get();

		// RuneSystem assets configuration
		EAssetTypeCategories::Type runeSystemCategory = assetTools.RegisterAdvancedAssetCategory(FName(TEXT("RuneSystem")), FText::FromString(TEXT("RuneSystem")));

		FRuneFilterActions* pActions = new FRuneFilterActions(runeSystemCategory);
		RuneFilterActions = MakeShareable(pActions);
		assetTools.RegisterAssetTypeActions(RuneFilterActions.ToSharedRef());
	}

	// PropertyEditor module
	{
		// import the PropertyEditor module...
		FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
		// to register our custom property
		PropertyModule.RegisterCustomPropertyTypeLayout(
		    FRuneTangibleAgentTemplate::StaticStruct()->GetFName(),
		    FOnGetPropertyTypeCustomizationInstance::CreateStatic(&FRuneTangibleAgentTemplateCustomization::MakeInstance));
		PropertyModule.NotifyCustomizationModuleChanged();
	}
}

void FRuneSystemEditor::ShutdownModule()
{
	UE_LOG(LogTemp, Warning, TEXT("RuneSystemEditor module shutdown"));

	if (FModuleManager::Get().IsModuleLoaded("AssetTools"))
	{
		IAssetTools& assetTools = FModuleManager::GetModuleChecked<FAssetToolsModule>("AssetTools").Get();
		assetTools.UnregisterAssetTypeActions(RuneFilterActions.ToSharedRef());
		RuneFilterActions.Reset();
	}

	if (FModuleManager::Get().IsModuleLoaded("PropertyEditor"))
	{
		// unregister properties when the module is shutdown
		FPropertyEditorModule& PropertyModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
		PropertyModule.UnregisterCustomPropertyTypeLayout("RuneTangibleAgentTemplate");

		PropertyModule.NotifyCustomizationModuleChanged();
	}
}