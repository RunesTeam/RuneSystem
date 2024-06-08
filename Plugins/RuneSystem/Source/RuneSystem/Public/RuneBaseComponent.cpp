


#include "RuneBaseComponent.h"
#include "RuneBehaviour.h"
#include "RuneCastStateMachine.h"
#include "RuneCompatible.h"
#include "RuneEffect.h"
#include "RuneInternalScheduler.h"
#include "RuneTask.h"


URuneBaseComponent::URuneBaseComponent()
{
	// do not tick, this component does only configuration stuff
	// it does not contain behaviour by itself (at least for now)
	PrimaryComponentTick.bCanEverTick = true;
}

void URuneBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// configure all relationships between components
	Configure();
}

void URuneBaseComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	if (IsValid())
	{
		if (runeInternalScheduler != nullptr)
		{
			int scheduledRuneConfigIndex = runeInternalScheduler->GetScheduledRuneConfigIndex();
			URuneTask* activeRuneTask = runeTasks[scheduledRuneConfigIndex];
				//runeConfigurations[scheduledRuneConfigIndex].runeCastStateMachine->runeTask;
			if (runeInternalScheduler->ScheduledRuneConfig(activeRuneTask)) 
			{
				// Reevaluate scheduled config index
				scheduledRuneConfigIndex = runeInternalScheduler->GetScheduledRuneConfigIndex();
				if (scheduledRuneConfigIndex >= 0 && scheduledRuneConfigIndex < runeConfigurations.Num()) 
				{
					runeConfigurations[scheduledRuneConfigIndex].runeCastStateMachine->TickCastStateMachine(DeltaTime, TickType, ThisTickFunction);
				}
			}
				
			if (activeRuneTask != nullptr)
			{
				activeRuneTask->PostEvaluationUpdate();
			}
		}
		else
		{
			// A valid rune always has, at least, one configuration, making this call safe
			runeConfigurations[0].runeCastStateMachine->TickCastStateMachine(DeltaTime, TickType, ThisTickFunction);
		}
	}	
}


void URuneBaseComponent::Press()
{
	if (IsValid())
	{
		int scheduledRuneConfigIndex = runeInternalScheduler ? runeInternalScheduler->GetScheduledRuneConfigIndex() : 0;
		runeConfigurations[scheduledRuneConfigIndex].runeCastStateMachine->SetPressed();
	}
}

void URuneBaseComponent::Release()
{
	if (IsValid())
	{
		int scheduledRuneConfigIndex = runeInternalScheduler ? runeInternalScheduler->GetScheduledRuneConfigIndex() : 0;
		runeConfigurations[scheduledRuneConfigIndex].runeCastStateMachine->SetReleased();
	}
}

bool URuneBaseComponent::IsValid() const
{
	bool isValid = runeConfigurations.Num() > 0;
	for (int i = 0; i < runeConfigurations.Num() && isValid; i++)
	{
		isValid &= runeConfigurations[i].isValid();
	}
	if (isValid && runeInternalScheduler != nullptr)
	{
		isValid &= runeConfigurations.Num() == runeTasks.Num();
	}
	return isValid;
}

void URuneBaseComponent::SetOwner(IRuneCompatible* owner)
{
	if (owner == nullptr) {
		return;
	}
	for (const FRuneConfiguration& rc : runeConfigurations)
	{
		for (const FRuneBehaviourWithEffects& rb : rc.runeBehavioursWithEffects)
		{
			if (rb.runeBehaviour == nullptr) continue;

			rb.runeBehaviour->runeOwner = owner->_getUObject(); // internal function, created in GENERATED_BODY() macro.
			for (URuneEffect* effect : rb.runeEffects)
			{
				if (effect != nullptr)
				{
					effect->SetInstigatorFilter(owner->GetRuneFilter());
					effect->SetInstigator(owner->GetController());
				}
			}
		}
	}
}

void URuneBaseComponent::Configure() const
{
	// if validation failed, do not configure
	if (!Validate())
	{
		return;
	}

	int index = 0;
	for (const FRuneConfiguration& rc : runeConfigurations)
	{
		TArray<URuneBehaviour*> behaviours;
		for (const FRuneBehaviourWithEffects& rb : rc.runeBehavioursWithEffects)
		{
			behaviours.Add(rb.runeBehaviour);
			for (URuneEffect* effect : rb.runeEffects)
			{
				rb.runeBehaviour->onApplyPulse.AddDynamic(effect, &URuneEffect::InternalApply);
				rb.runeBehaviour->onRevertPulse.AddDynamic(effect, &URuneEffect::InternalRevert);
			}
		}
		rc.runeCastStateMachine->SetLinkedBehaviour(behaviours);
		if (runeTasks[index] != nullptr)
		{
			runeTasks[index]->Configure(rc);
		}
		index++;
	}
	if (runeInternalScheduler != nullptr)
	{
		runeInternalScheduler->Configure(runeConfigurations.Num());
	}
}

bool URuneBaseComponent::Validate() const
{
	// log everything that is wrong, then return if it is valid or not.
	bool valid = IsValid();
	if (!valid)
	{
		//LOG_WARNING("Error while validating '%s', one or more runeBehaviours are not properly set", *GetOwner()->GetName());
	}

	return valid;
}

#if WITH_EDITOR
void URuneBaseComponent::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
	Super::PostEditChangeProperty(PropertyChangedEvent);
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(URuneBaseComponent, runeConfigurations))
	{
		if (runeConfigurations.Num() <= 0)
		{
			runeTasks.Init(nullptr, runeConfigurations.Num());
		}
		else
		{
			TArray<URuneTask*> aux; 
			aux.Init(nullptr, runeConfigurations.Num());
			int index = 0;
			for (int i = 0; i < aux.Num() && i < runeTasks.Num(); i++)
			{
				aux[i] = runeTasks[i];

			}
			runeTasks = aux;
		}
	}
}

void URuneBaseComponent::PostEditChangeChainProperty(FPropertyChangedChainEvent& PropertyChangedChainEvent)
{
	Super::PostEditChangeChainProperty(PropertyChangedChainEvent);
	if (PropertyChangedChainEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(FRuneConfiguration, runeCastStateMachine))
	{
		int32 index = PropertyChangedChainEvent.GetArrayIndex(GET_MEMBER_NAME_CHECKED(URuneBaseComponent, runeConfigurations).ToString());
		if (index >= 0 && index < runeConfigurations.Num())
		{
			FRuneConfiguration& rc = runeConfigurations[index];
			if (rc.runeCastStateMachine != nullptr)
			{
				TArray<FRuneBehaviourWithEffects> aux;
				if (rc.runeBehavioursWithEffects.Num() <= 0)
				{
					rc.runeBehavioursWithEffects.Init(FRuneBehaviourWithEffects(), rc.runeCastStateMachine->GetBehaviourSlotCount());
				}
				else
				{
					aux.Init(FRuneBehaviourWithEffects(), rc.runeCastStateMachine->GetBehaviourSlotCount());
					int i = 0;
					for (FRuneBehaviourWithEffects& rb : rc.runeBehavioursWithEffects)
					{
						if (i == aux.Num())
						{
							break;
						}
						aux[i] = rb;
						++i;
					}
					rc.runeBehavioursWithEffects = aux;
				}
			}
		}
	}
}
#endif
