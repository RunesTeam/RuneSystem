
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RuneInternalScheduler.generated.h"

class URuneTask;

UCLASS(Abstract, BlueprintType, Blueprintable, EditInlineNew)
class RUNESYSTEM_API URuneInternalScheduler : public UObject
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URuneInternalScheduler();

public:
	// TODO: Passing this int seems oddly specific, is this right?
	virtual void Configure(int maxRuneConfigIndex);

	/**
	 * Evaluates the task of the last scheduled configuration to control execution the flow of the rune
	 * 
	 * @param activeRuneTask Task to be evaluated, linked to the last scheduled configuration
	 * @return true if the scheduled configuration should be ticked
	 */
	virtual bool ScheduledRuneConfig(const URuneTask* activeRuneTask);

	/**
	 * Blueprint version of ScheduledRuneConfig()
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "ScheduledRuneConfig"))
	bool ReceiveScheduledRuneConfig(const URuneTask* activeRuneTask);

	UFUNCTION(BlueprintCallable)
	bool IsValid() const;

	UFUNCTION(BlueprintCallable)
	inline int GetScheduledRuneConfigIndex() const { return scheduledRuneConfigIndex; };

protected:
	UPROPERTY(BlueprintReadWrite)
	int scheduledRuneConfigIndex;
	UPROPERTY(BlueprintReadWrite)
	int maxRuneConfigIndex;
};
