


#include "EoTComponent.h"
#include "RuneEffect.h"
#include "TimerManager.h"


UEoTComponent::UEoTComponent() : runeEffectClass(nullptr),
                                 runeEffect(nullptr),
                                 ticks(5),
                                 duration(5.0f),
                                 trimTickDistribution(true),
                                 tickRate(0.5f),
                                 _timePerTick(-1.0f),
                                 _remainingTicks(0),
                                 _timeHandle()
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = false;
}

void UEoTComponent::BeginPlay()
{
	Super::BeginPlay();

	if (runeEffect == nullptr && runeEffectClass == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EoTComponent] BeginPlay(): runeEffect nor runeEffectClass are set. Destroying component"));
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
		UE_LOG(LogTemp, Warning, TEXT("[EoTComponent] BeginPlay(): runeEffect is nullptr. Destroying component."));
		DestroyComponent();
		return;
	}

	// auxiliar variables initialization
	// If duration is set to a negative number the number of tick will be indefinite
	if (duration < 0)
	{
		_timePerTick = tickRate;
		// With a negative amount of remaining ticks, timer will never stop on its own
		_remainingTicks = -1;
	}
	else
	{
		_timePerTick = trimTickDistribution ? duration / (ticks - 1) : duration / (ticks + 1);
		_remainingTicks = ticks;
	}

	// initialize timer
	const float inicialDelay = trimTickDistribution ? 0.0f : _timePerTick;
	GetOwner()->GetWorldTimerManager().SetTimer(_timeHandle, this, &UEoTComponent::ApplyTickEffect, _timePerTick, true, inicialDelay);
}

//void UEoTComponent::Configure(const TSubclassOf<URuneEffect>& inEffectClass, AActor* instigator, uint32 inTicks, float inDuration, bool inTrimTickDistribution, float inTickRate)
//{
//	if (_timeHandle.IsValid())
//	{
//		UE_LOG(LogTemp, Warning, TEXT("[EoTComponent] Configure(): Cannot configure when EoT has already began."));
//		return;
//	}
//
//	runeEffectClass = inEffectClass;
//	runeEffect = nullptr;
//	_instigator = instigator;
//	ticks = inTicks;
//	duration = inDuration;
//	trimTickDistribution = inTrimTickDistribution;
//	tickRate = inTickRate;
//}

void UEoTComponent::Configure(URuneEffect* inEffect, AController* instigator, uint32 inTicks, float inDuration, bool inTrimTickDistribution, float inTickRate)
{
	if (_timeHandle.IsValid())
	{
		UE_LOG(LogTemp, Warning, TEXT("[EoTComponent] Configure(): Cannot configure when EoT has already began."));
		return;
	}

	runeEffectClass = nullptr;
	_instigator = instigator;
	// Copy rune effect instance so that it stays alive all duration
	runeEffect = DuplicateObject<URuneEffect>(inEffect, this);
	ticks = inTicks;
	duration = inDuration;
	trimTickDistribution = inTrimTickDistribution;
	tickRate = inTickRate;
}

void UEoTComponent::RemoveFromOwner(AActor* owner)
{
	if (owner != nullptr && owner == GetOwner())
	{
		this->DestroyComponent();
	}
}

void UEoTComponent::ApplyTickEffect()
{
	AActor* actor = GetOwner();
	if (actor == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EoTComponent] ApplyTickEffect() error: Actor owner is nullptr"));
		return;
	}

	if (runeEffect == nullptr)
	{
		UE_LOG(LogTemp, Warning, TEXT("[EoTComponent] ApplyTickEffect() error: runeEffect is nullptr"));
		return;
	}

	//UE_LOG(LogTemp, Display, TEXT("[EoTComponent] EoT effect applied"));
	runeEffect->ApplyEffectInstant(_instigator, _instigator, actor);
	--_remainingTicks;

	// check if it is the last tick
	// if so, invalidates timer and destroy component
	if (_remainingTicks == 0)
	{
		GetOwner()->GetWorldTimerManager().ClearTimer(_timeHandle);
		_timeHandle.Invalidate();

		DestroyComponent();
	}
}
