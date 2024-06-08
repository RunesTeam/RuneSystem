// Fill out your copyright notice in the Description page of Project Settings.


#include "RuneSystem/InternalSchedulers/ExtendedRuneInternalScheduler.h"
#include "RuneSystem/Tasks/ExtendedRuneTask.h"
#include "RuneTask.h"

bool UExtendedRuneInternalScheduler::ScheduledRuneConfig(const URuneTask* activeRuneTask)
{
	bool shouldScheduleTick = IsValid();

	if (shouldScheduleTick && activeRuneTask != nullptr)
	{
		switch (activeRuneTask->GetTaskValue())
		{
		case ERuneTaskValue::RUNNING:
			break;
		case ERuneTaskValue::HOLD:
			shouldScheduleTick = false;
			break;
		case ERuneTaskValue::FAILURE:
			scheduledRuneConfigIndex = 0;
			break;
		case ERuneTaskValue::SUCCESS:
			scheduledRuneConfigIndex++;
			scheduledRuneConfigIndex %= maxRuneConfigIndex;
			break;
		default:
			break;
		}
#if 1
		if (activeRuneTask->taskValue == EUserDefinedRuneTaskReturnValue::SECOND)
		{
			shouldScheduleTick = false;
		}
		else if (activeRuneTask->taskValue == EUserDefinedRuneTaskReturnValue::THIRD)
		{
			scheduledRuneConfigIndex = 0;
		}
		else if (activeRuneTask->taskValue == EUserDefinedRuneTaskReturnValue::FOURTH)
		{
			scheduledRuneConfigIndex++;
			scheduledRuneConfigIndex %= maxRuneConfigIndex;
		}
#else
		switch (activeRuneTask->taskValue.Get<EUserDefinedRuneTaskReturnValue>())
		{
		case EUserDefinedRuneTaskReturnValue::FIRST:
			break;
		case EUserDefinedRuneTaskReturnValue::SECOND:
			shouldScheduleTick = false;
			break;
		case EUserDefinedRuneTaskReturnValue::THIRD:
			scheduledRuneConfigIndex = 0;
			break;
		case  EUserDefinedRuneTaskReturnValue::FOURTH:
			scheduledRuneConfigIndex++;
			scheduledRuneConfigIndex %= maxRuneConfigIndex;
			break;
		default:
			break;
		}
#endif

	}
	return shouldScheduleTick;
}
