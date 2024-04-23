


#include "RuneTangibleAgent.h"
#include "RunePreviewAgent.h"
#include "RuneEffect.h"


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
	FBooleanPtr successPtr({ &success });
	for (URuneEffect* effect : attachedRuneEffects)
	{
		if (effect != nullptr)
		{
			effect->InternalApply(effect->GetInstigator(), this, actor, successPtr);
		}
	}
	onApplyEffects.Broadcast(actor, success);

	return success;
}

bool ARuneTangibleAgent::TryRevertEffects(AActor* actor)
{
	if(actor == nullptr) return false;

	bool success = false;
	FBooleanPtr successPtr({ &success });
	for (URuneEffect* effect : attachedRuneEffects)
	{
		if (effect != nullptr)
		{
			effect->InternalRevert(actor, successPtr);
		}
	}
	onRevertEffects.Broadcast(actor, success);

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
			result |= !effect->Filter(*actor);
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