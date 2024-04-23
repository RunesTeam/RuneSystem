


#include "StatusComponent.h"
#include "RuneEffect.h"
#include "TimerManager.h"


UStatusComponent::UStatusComponent() :
	runeEffectClass(nullptr),
	runeEffect(nullptr),
	duration(5.0f),
	_timeHandle()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;

}

void UStatusComponent::BeginPlay()
{
	Super::BeginPlay();

	if (runeEffect == nullptr && runeEffectClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] BeginPlay(): runeEffect nor runeEffectClass are set. Destroying component"));
		DestroyComponent();
		return;
	}

	// initialize if RuneEffectClass is set
	if (runeEffectClass != nullptr)
	{
		UActorComponent* component = GetOwner()->AddComponentByClass(runeEffectClass, false, FTransform::Identity, false);
		runeEffect = Cast<URuneEffect>(component);
	}

	if (runeEffect == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] BeginPlay(): runeEffect is nullptr. Destroying component."));
		DestroyComponent();
		return;
	}

	// apply effect
	BeginStatusEffect();

	// revert effect after Duration seconds
	GetOwner()->GetWorldTimerManager().SetTimer(_timeHandle, this, &UStatusComponent::EndStatusEffect, duration, false);
}

//void UStatusComponent::Configure(const TSubclassOf<URuneEffect>& inEffectClass, float inDuration)
//{
//	if (_timeHandle.IsValid())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] Configure(): Cannot configure when Status has already began."));
//		return;
//	}
//
//	this->runeEffectClass = inEffectClass;
//	this->runeEffect = nullptr;
//	this->duration = inDuration;
//}

void UStatusComponent::Configure(URuneEffect* inEffect, AController* instigator, float inDuration)
{
	if (_timeHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] Configure(): Cannot configure when Status has already began."));
		return;
	}

	this->runeEffectClass = nullptr;
	_instigator = instigator;
	// Copy rune effect instance so that it stays alive all duration
	runeEffect = DuplicateObject<URuneEffect>(inEffect, this);
	this->duration = inDuration;
}

void UStatusComponent::BeginStatusEffect()
{
	AActor* actor = GetOwner();
	if (actor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] BeginStatusEffect() error: Actor owner is nullptr"));
		return;
	}

	if (runeEffect == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] BeginStatusEffect() error: runeEffect is nullptr"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("[StatusComponent] Status effect applied"));
	runeEffect->ApplyEffectInstant(_instigator, _instigator, actor);
}

void UStatusComponent::EndStatusEffect()
{
	AActor* actor = GetOwner();
	if (actor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] EndStatusEffect() error: Actor owner is nullptr"));
		return;
	}

	if (runeEffect == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[StatusComponent] EndStatusEffect() error: runeEffect is nullptr"));
		return;
	}

	UE_LOG(LogTemp, Display, TEXT("[StatusComponent] Status effect reverted"));
	
	runeEffect->RevertEffectInstant(actor);
	runeEffect->DestroyComponent();
	_timeHandle.Invalidate();
	DestroyComponent();
}
