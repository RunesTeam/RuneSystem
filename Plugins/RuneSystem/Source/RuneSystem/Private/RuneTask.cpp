

#include "RuneTask.h"

// Sets default values for this component's properties
URuneTask::URuneTask() : currentReturnValue(ERuneTaskReturnValue::RUNNING)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;

	// ...
}

ERuneTaskReturnValue URuneTask::Evaluate() const
{
	return currentReturnValue;
}


// Called when the game starts
void URuneTask::BeginPlay()
{
	Super::BeginPlay();

	// ...
	
}


// Called every frame
void URuneTask::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// ...
}

void URuneTask::Configure(const URuneBaseComponent* runeBaseComponent)
{
	// by default calls the blueprint version
	ReceiveConfigure(runeBaseComponent);
}

