
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RuneBaseComponent.h"
#include "RuneTask.generated.h"

UENUM(BlueprintType)
enum class ERuneTaskValue : uint8
{
	INVALID = 0			 UMETA(Hidden),
	SUCCESS = 1,
	FAILURE,
	RUNNING,
	HOLD,
	FIRST_ENUM = SUCCESS	UMETA(Hidden),
	LAST_ENUM = HOLD		UMETA(Hidden)
};

using ExtendedTaskValue = ExtendedEnum<ERuneTaskValue>;

class URuneCastStateMachine;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class RUNESYSTEM_API URuneTask : public UObject
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URuneTask();

	/**
	 * Sets up all listeners necessary for the task to have an updated value
	 * based on its linked runeConfiguration
	 *
	 * @param runeConfiguration Set of CastStateMachine, Behaviours and Effects linked to the task
	 */
	UFUNCTION(BlueprintCallable)
	virtual void Configure(const FRuneConfiguration& runeConfiguration);

	/**
	 * Blueprint-implementable version of Configure()
	 *
	 * @param runeConfiguration
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Configure"))
	void ReceiveConfigure(const FRuneConfiguration& runeConfiguration);

	/**
	 * Functionality that gets call every frame after the current value of the task has been evaluated	 * 
	 */
	virtual void PostEvaluationUpdate();

	/**
	 * Blueprint-implementable version of PostEvaluationUpdate()	 *
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "PostEvaluationUpdate"))
	void ReceivePostEvaluationUpdate();

	UFUNCTION(BlueprintCallable)
	ERuneTaskValue GetTaskValue() const { return taskValue.Get<ERuneTaskValue>(); };

	UFUNCTION(BlueprintCallable)
	virtual void SetTaskValue(ERuneTaskValue newValue) { taskValue = newValue; };

public:
	ExtendedTaskValue taskValue;
};
