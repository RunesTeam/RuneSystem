


#include "RuneBaseComponent.h"
#include "RuneBehaviour.h"
#include "RuneCastStateMachine.h"
#include "RuneCompatible.h"
#include "RuneEffect.h"


URuneBaseComponent::URuneBaseComponent() : runeCastStateMachine(nullptr)
{
	// do not tick, this component does only configuration stuff
	// it does not contain behaviour by itself (at least for now)
	PrimaryComponentTick.bCanEverTick = false;
}

void URuneBaseComponent::BeginPlay()
{
	Super::BeginPlay();

	// configure all relationships between components
	Configure();
}


void URuneBaseComponent::Press()
{
	if (IsValid())
	{
		runeCastStateMachine->SetPressed();
	}
}

void URuneBaseComponent::Release()
{
	if (IsValid())
	{
		runeCastStateMachine->SetReleased();
	}
}

bool URuneBaseComponent::IsValid() const
{
	bool isValid = true;
	for (const FRuneBehaviourWithEffects& rb : runeBehavioursWithEffects)
	{
		isValid &= rb.isValid();
	}

	return runeCastStateMachine != nullptr && isValid;
}

void URuneBaseComponent::SetOwner(IRuneCompatible* owner)
{
	if (owner == nullptr) {
		return;
	}
	for (const FRuneBehaviourWithEffects& rb : runeBehavioursWithEffects)
	{
		if (rb.runeBehaviour == nullptr) continue;

		rb.runeBehaviour->runeOwner = owner->_getUObject(); // internal function, created in GENERATED_BODY() macro.
		for (URuneEffect* effect : rb.runeEffects)
		{
			if (effect != nullptr)
			{
				/*effect->SetInstigatorFilter(owner->GetRuneFilter());
				effect->SetInstigator(owner->GetController());*/
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

	TArray<URuneBehaviour*> behaviours;
	for (const FRuneBehaviourWithEffects& rb : runeBehavioursWithEffects)
	{
		behaviours.Add(rb.runeBehaviour);
		for (URuneEffect* effect : rb.runeEffects)
		{
		/*	rb.runeBehaviour->onApplyPulse.AddDynamic(effect, &URuneEffect::Activate);
			rb.runeBehaviour->onRevertPulse.AddDynamic(effect, &URuneEffect::Deactivate);*/
		}
	}
	runeCastStateMachine->SetLinkedBehaviour(behaviours);
}

bool URuneBaseComponent::Validate() const
{
	// log everything that is wrong, then return if it is valid or not.
	if (runeCastStateMachine == nullptr)
	{
		//LOG_WARNING("Error while validating '%s', runeCastStateMachine component not found", *GetOwner()->GetName());
	}
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
	TArray<FRuneBehaviourWithEffects> aux;
	if (PropertyChangedEvent.GetPropertyName() == GET_MEMBER_NAME_CHECKED(URuneBaseComponent, runeCastStateMachine) && runeCastStateMachine != nullptr)
	{
		if (runeBehavioursWithEffects.Num() <= 0)
		{
			runeBehavioursWithEffects.Init(FRuneBehaviourWithEffects(), runeCastStateMachine->GetBehaviourSlotCount());
		}
		else
		{
			aux.Init(FRuneBehaviourWithEffects(), runeCastStateMachine->GetBehaviourSlotCount());
			int index = 0;
			for (FRuneBehaviourWithEffects& rb : runeBehavioursWithEffects)
			{
				if (index == aux.Num())
				{
					break;
				}
				aux[index] = rb;
				++index;
			}
			runeBehavioursWithEffects = aux;
		}
	}
}
#endif
