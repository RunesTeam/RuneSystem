


#include "EoTRuneEffectApplicationMode.h"
#include "RuneEffect.h"
#include "RuneSystem.h"


UEoTRuneEffectApplicationMode::UEoTRuneEffectApplicationMode() :
	Duration(5.0f),
	Ticks(5u),
	TrimTickDistribution(true),
	TickRate(1.0f)
{
}

#if WITH_EDITOR
bool UEoTRuneEffectApplicationMode::CanEditChange(const FProperty* InProperty) const
{
	const bool result = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UEoTRuneEffectApplicationMode, Ticks))
	{
		return result && Duration > 0.0f;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UEoTRuneEffectApplicationMode, TrimTickDistribution))
	{
		return result && Duration > 0.0f;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UEoTRuneEffectApplicationMode, TickRate))
	{
		return result && Duration <= 0.0f;
	}

	return result;
}
#endif

void UEoTRuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	// auxiliary variables initialization
	// If duration is set to a negative number the number of tick will be indefinite
	const bool bLoop = Duration < 0.0f;
	const float TimePerTick = bLoop
		? TickRate
		: TrimTickDistribution ? Duration / (Ticks - 1) : Duration / (Ticks + 1);
	const float InicialDelay = TrimTickDistribution ? 0.0f : TimePerTick;

	UEoTRuneEffectApplicationData* ApplicationData = NewObject<UEoTRuneEffectApplicationData>();
	check(IsValid(ApplicationData));
	// Timer that applies ticks
	{
		Handle.Payload.Target->GetWorldTimerManager().SetTimer(
			ApplicationData->TickTimerHandle,
			FTimerDelegate::CreateLambda([=, this]()
				{
					SubmitApply(Handle);
				}),
			TimePerTick,
			true,
			InicialDelay);
	}

	// Timer that deactivates on finish
	if (!bLoop)
	{
		Handle.Payload.Target->GetWorldTimerManager().SetTimer(
			ApplicationData->DeactivationTimerHandle,
			FTimerDelegate::CreateLambda([=, this]()
				{
					ApplicationData->DeactivationTimerHandle.Invalidate();
					SubmitDeactivation(Handle);
				}),
			Duration,
			false);
	}

	URuneSystem::SetEffectHandleApplicationData(Handle, ApplicationData);
}

void UEoTRuneEffectApplicationMode::HandleEffectDeactivation(const FRuneEffectHandle& Handle)
{
	UEoTRuneEffectApplicationData* ApplicationData = Cast< UEoTRuneEffectApplicationData>(URuneSystem::GetEffectHandleApplicationData(Handle));
	check(IsValid(ApplicationData));

	if (ApplicationData->TickTimerHandle.IsValid())
	{
		Handle.Payload.Target->GetWorldTimerManager().ClearTimer(ApplicationData->TickTimerHandle);
	}
}
