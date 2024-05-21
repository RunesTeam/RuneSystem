


#include "RuneTangibleAgent.h"
#include "RunePreviewAgent.h"
#include "Effect/RuneEffect.h"
#include "RuneSystem.h"


ARuneTangibleAgent::ARuneTangibleAgent() : 
	duration(30.0f),
	previewAgentClass(nullptr),
	attachedRuneEffects()
{
	// Set this actor to call Tick() every frame. You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

#if WITH_EDITOR
bool ARuneTangibleAgent::CanEditChange(const FProperty* InProperty) const
{
	const bool result = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(ARuneTangibleAgent, duration))
	{
		return result && !HasActorBegunPlay();
	}

	return result;
}
#endif

void ARuneTangibleAgent::BeginPlay()
{
	Super::BeginPlay();

	if (duration >= 0.0f)
	{
		SetLifeSpan(duration);
	}
}

void ARuneTangibleAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

bool ARuneTangibleAgent::TryApplyEffects(AActor* actor)
{
	if(actor == nullptr) return false;

	bool success = false;
	for (URuneEffect* effect : attachedRuneEffects)
	{
		if (effect != nullptr)
		{
			FRuneEffectPayload Payload;
			Payload.Target = actor;
			Payload.Instigator = GetInstigator();
			Payload.Causer = this;

			URuneSystem::ActivateEffect(effect, Payload);
		}
	}
	onApplyEffects.Broadcast(actor, success);

	// TODO: Return array of valid handles
	return success;
}

bool ARuneTangibleAgent::TryRevertEffects(AActor* actor)
{
	if(actor == nullptr) return false;

	bool success = false;
	for (URuneEffect* effect : attachedRuneEffects)
	{
		if (effect != nullptr)
		{
			TArray<FRuneEffectHandle> Handles = URuneSystem::GetEffectHandlesByPredicate(
				this,
				[actor, this](const FRuneEffectHandle& Handle)
				{
					return attachedRuneEffects.Contains(Handle.Effect) && Handle.Payload.Target == actor;
				});

			for (const FRuneEffectHandle& Handle : Handles)
			{
				URuneSystem::DeactivateEffect(Handle);
			}
		}
	}
	onRevertEffects.Broadcast(actor, success);

	// TODO: Return array of valid handles
	return success;
}

bool ARuneTangibleAgent::CheckForApplicableEffects(AActor* actor)
{
	if (actor == nullptr) return false;

	bool result = false;
	for (URuneEffect* effect : attachedRuneEffects)
	{
		if (effect != nullptr)
		{
			// TODO: return back the filtering somehow
			//result |= !effect->Filter(*actor);
		}
	}
	return result;
}

void ARuneTangibleAgent::SetAttachedRuneEffects(TArray<class UObject*> runeEffects)
{
	for (UObject* object : runeEffects)
	{
		URuneEffect* effect = Cast<URuneEffect>(object);
		if (effect != nullptr)
		{
			URuneEffect* copy = DuplicateObject<URuneEffect>(effect, this);
			attachedRuneEffects.Add(copy);
		}
	}
}

TSubclassOf<ARunePreviewAgent> ARuneTangibleAgent::GetPreviewAgentClass() const
{
	return previewAgentClass;
}