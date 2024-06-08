

#include "RuneInternalScheduler.h"

// Sets default values for this component's properties
URuneInternalScheduler::URuneInternalScheduler() : scheduledRuneConfigIndex(0), maxRuneConfigIndex(0)
{
}

void URuneInternalScheduler::Configure(int inMaxRuneConfigIndex)
{
	maxRuneConfigIndex = inMaxRuneConfigIndex;
}

bool URuneInternalScheduler::ScheduledRuneConfig(const URuneTask* activeRuneTask)
{
	return ReceiveScheduledRuneConfig(activeRuneTask);
}

bool URuneInternalScheduler::IsValid() const
{
	return maxRuneConfigIndex > scheduledRuneConfigIndex;
}

