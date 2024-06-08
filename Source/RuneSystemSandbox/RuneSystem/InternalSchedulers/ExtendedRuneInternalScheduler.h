// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuneInternalScheduler.h"
#include "ExtendedRuneInternalScheduler.generated.h"

/**
 * 
 */
UCLASS()
class RUNESYSTEMSANDBOX_API UExtendedRuneInternalScheduler : public URuneInternalScheduler
{
	GENERATED_BODY()

	virtual bool ScheduledRuneConfig(const URuneTask* activeRuneTask) override;

};
