// Fill out your copyright notice in the Description page of Project Settings.


#include "RuneSystem/Tasks/ExtendedRuneTask.h"

UExtendedRuneTask::UExtendedRuneTask()
{
	taskValue = EUserDefinedRuneTaskReturnValue::FIRST;
}

EUserDefinedRuneTaskReturnValue UExtendedRuneTask::GetTaskEvaluationExtended() const
{
	return taskValue.Get<EUserDefinedRuneTaskReturnValue>();
}
