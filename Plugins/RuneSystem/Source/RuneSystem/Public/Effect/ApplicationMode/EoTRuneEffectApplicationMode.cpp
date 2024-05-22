


#include "EoTRuneEffectApplicationMode.h"
#include "RuneSystem.h"
#include "Effect/RuneEffect.h"
#include "Effect/RuneEffectHandle.h"


UEoTRuneEffectApplicationMode::UEoTRuneEffectApplicationMode() :
	Duration(5.0f),
	Ticks(5u),
	TrimTickDistribution(true),
	TickRate(1.0f),
	bStackResetsDuration(true),
	bStackSyncsApplication(true)
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
	const bool bLoop = Duration < 0.0f;
	auto ConfigureTickTimer = [this, bLoop](const FRuneEffectHandle& InHandle, FTimerHandle& TimerHandle)
		{
			// auxiliary variables initialization
			// If duration is set to a negative number the number of tick will be indefinite
			const float TimePerTick = bLoop
				? TickRate
				: TrimTickDistribution ? Duration / (Ticks - 1) : Duration / (Ticks + 1);
			const float InicialDelay = TrimTickDistribution ? 0.0f : TimePerTick;

			InHandle.Payload.Target->GetWorldTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([=, this]()
					{
						SubmitApply(InHandle);
					}),
				TimePerTick,
				true,
				InicialDelay);
		};

	auto ConfigureDeactivationTimer = [this](const FRuneEffectHandle& InHandle, FTimerHandle& TimerHandle)
		{
			InHandle.Payload.Target->GetWorldTimerManager().SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([=, this]() mutable
					{
						TimerHandle.Invalidate();
						SubmitDeactivation(InHandle);
					}),
				Duration,
				false);
		};

	Super::HandleEffectActivation(Handle);

	// handle stack application
	TArray<FRuneEffectHandle> ActiveApplicationHandles = URuneSystem::GetEffectHandlesByPredicate(
		this,
		[&Handle](const FRuneEffectHandle& InHandle)
		{
			return InHandle.ID != Handle.ID
				&& InHandle.Effect == Handle.Effect
				&& InHandle.Payload.Target == Handle.Payload.Target
				&& InHandle.Payload.Instigator == Handle.Payload.Instigator;
		});

	for (const FRuneEffectHandle& PrevHandle : ActiveApplicationHandles)
	{
		UEoTRuneEffectApplicationData* PreviousApplicationData = Cast<UEoTRuneEffectApplicationData>(URuneSystem::GetEffectHandleApplicationData(this, PrevHandle));
		check(IsValid(PreviousApplicationData));

		// sync apply calls 
		if (bStackSyncsApplication)
		{
			ConfigureTickTimer(PrevHandle, PreviousApplicationData->TickTimerHandle);
		}

		// reset duration timer if needed
		if (bStackResetsDuration && !bLoop)
		{
			ConfigureDeactivationTimer(PrevHandle, PreviousApplicationData->DeactivationTimerHandle);
		}
	}

	// Data for the new activation
	UEoTRuneEffectApplicationData* ApplicationData = NewObject<UEoTRuneEffectApplicationData>();
	check(IsValid(ApplicationData));

	// Timer that applies ticks
	ConfigureTickTimer(Handle, ApplicationData->TickTimerHandle);

	// Timer that deactivates on finish
	if (!bLoop)
	{
		ConfigureDeactivationTimer(Handle, ApplicationData->DeactivationTimerHandle);
	}

	URuneSystem::SetEffectHandleApplicationData(this, Handle, ApplicationData);
}

void UEoTRuneEffectApplicationMode::HandleEffectDeactivation(const FRuneEffectHandle& Handle)
{
	Super::HandleEffectDeactivation(Handle);

	UEoTRuneEffectApplicationData* ApplicationData = Cast<UEoTRuneEffectApplicationData>(URuneSystem::GetEffectHandleApplicationData(this, Handle));
	check(IsValid(ApplicationData));

	if (ApplicationData->TickTimerHandle.IsValid())
	{
		Handle.Payload.Target->GetWorldTimerManager().ClearTimer(ApplicationData->TickTimerHandle);
	}
}
