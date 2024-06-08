
#include "RuneTask.h"


// Sets default values for this component's properties
URuneTask::URuneTask() : taskValue(ERuneTaskValue::RUNNING)
{
}

void URuneTask::PostEvaluationUpdate()
{
	// by default calls the blueprint version
	ReceivePostEvaluationUpdate();
}

void URuneTask::Configure(const FRuneConfiguration& runeConfiguration)
{
	// by default calls the blueprint version
	ReceiveConfigure(runeConfiguration);
}

