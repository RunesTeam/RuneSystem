// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuneTask.h"
#include "ExtendedRuneTask.generated.h"


UENUM(BlueprintType)
enum class EUserDefinedRuneTaskReturnValue : uint8
{
	INVALID = 0			 UMETA(Hidden),
	FIRST = 5,
	SECOND,
	THIRD,
	FOURTH,
	FIRST_ENUM = FIRST	UMETA(Hidden),
	LAST_ENUM = FOURTH	UMETA(Hidden)
};

EXTEND_ENUM(ERuneTaskValue, EUserDefinedRuneTaskReturnValue)

UCLASS(Abstract)
class RUNESYSTEMSANDBOX_API UExtendedRuneTask : public URuneTask
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UExtendedRuneTask();

	UFUNCTION(BlueprintCallable)
	EUserDefinedRuneTaskReturnValue GetTaskEvaluationExtended() const;

	UFUNCTION(BlueprintCallable)
	void SetTaskEvaluationExtended(EUserDefinedRuneTaskReturnValue newValue) {
		taskValue = newValue;
	};
};
