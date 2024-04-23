// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "Modules/ModuleInterface.h"
#include "IAssetTypeActions.h"


class FRuneSystemEditor : public IModuleInterface
{
public:
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

private:
    TSharedPtr<IAssetTypeActions> RuneFilterActions;
    
};
