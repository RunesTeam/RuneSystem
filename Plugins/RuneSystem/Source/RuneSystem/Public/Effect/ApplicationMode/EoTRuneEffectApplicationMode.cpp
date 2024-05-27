


#include "EoTRuneEffectApplicationMode.h"
#include "RuneSystem.h"
#include "Effect/RuneEffect.h"
#include "Effect/RuneEffectHandle.h"


UEoTRuneEffectApplicationMode::UEoTRuneEffectApplicationMode() :
	Duration(5.0f),
	Ticks(5u),
	TrimTickDistribution(true),
	TickRate(1.0f),
	StackDurationPolicy(EStackDurationPolicy::NONE),
	StackApplicationPolicy(EStackApplicationPolicy::UNSYNCRONIZED),
	MaxStackCount(0u)
{
}

#if WITH_EDITOR
bool UEoTRuneEffectApplicationMode::CanEditChange(const FProperty* InProperty) const
{
	const bool Result = Super::CanEditChange(InProperty);

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UEoTRuneEffectApplicationMode, Ticks))
	{
		return Result && Duration > 0.0f;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UEoTRuneEffectApplicationMode, TrimTickDistribution))
	{
		return Result && Duration > 0.0f;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(UEoTRuneEffectApplicationMode, TickRate))
	{
		return Result && Duration <= 0.0f;
	}

	return Result;
}
#endif

void UEoTRuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	// If duration is set to a negative number the number of tick will be indefinite
	// Auxiliary variables initialization
	const bool bLoop = Duration < 0.0f;
	const float TimePerTick = bLoop
		? TickRate
		: TrimTickDistribution ? Duration / (Ticks - 1) : Duration / (Ticks + 1);
	const float InicialDelay = TrimTickDistribution ? 0.0f : TimePerTick;

	auto ConfigureTickTimer = [&, this](const FRuneEffectHandle& InHandle, FTimerHandle& TimerHandle, float* InInitialDelay)
		{
			FTimerManager& TimerManager = InHandle.Payload.Target->GetWorldTimerManager();
			TimerManager.SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([=, this]()
					{
						SubmitApply(InHandle);
					}),
				TimePerTick,
				true,
				InInitialDelay != nullptr ? *InInitialDelay : InicialDelay);
		};

	auto ConfigureDeactivationTimer = [this](const FRuneEffectHandle& InHandle, FTimerHandle& TimerHandle, float* InDuration)
		{
			FTimerManager& TimerManager = InHandle.Payload.Target->GetWorldTimerManager();
			TimerManager.SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([=, this]() mutable
					{
						TimerHandle.Invalidate();
						SubmitDeactivation(InHandle);
					}),
				InDuration != nullptr ? *InDuration : Duration,
				false);
		};

	Super::HandleEffectActivation(Handle);

	// Data for the new activation
	UEoTRuneEffectApplicationData* ApplicationData = NewObject<UEoTRuneEffectApplicationData>();
	check(IsValid(ApplicationData));

	// Timer that applies ticks
	ConfigureTickTimer(Handle, ApplicationData->TickTimerHandle, nullptr);

	// Timer that deactivates on finish
	if (!bLoop)
	{
		ConfigureDeactivationTimer(Handle, ApplicationData->DeactivationTimerHandle, nullptr);
	}

	URuneSystem::SetEffectHandleApplicationData(this, Handle, ApplicationData);

	// Handle stack application
	const bool bCanBeStacked =
		StackDurationPolicy != EStackDurationPolicy::NONE ||
		StackApplicationPolicy != EStackApplicationPolicy::UNSYNCRONIZED;
	if (bCanBeStacked)
	{
		TArray<FRuneEffectHandle> ActiveApplicationHandles = URuneSystem::GetEffectHandlesByPredicate(
			this,
			[&Handle](const FRuneEffectHandle& InHandle)
			{
				return InHandle.ID != Handle.ID
					&& InHandle.Effect == Handle.Effect
					&& InHandle.Payload.Target == Handle.Payload.Target
					&& InHandle.Payload.Instigator == Handle.Payload.Instigator;
			});

		bool bLocalStackHandled = false;
		for (const FRuneEffectHandle& PrevHandle : ActiveApplicationHandles)
		{
			UEoTRuneEffectApplicationData* PreviousApplicationData = Cast<UEoTRuneEffectApplicationData>(URuneSystem::GetEffectHandleApplicationData(this, PrevHandle));
			check(IsValid(PreviousApplicationData));

			// Sync apply calls 
			if (!bLocalStackHandled && StackApplicationPolicy == EStackApplicationPolicy::SYNCRONIZED)
			{
				FTimerManager& TimerManager = Handle.Payload.Target->GetWorldTimerManager();
				const float ElapsedTime = TimerManager.GetTimerElapsed(PreviousApplicationData->TickTimerHandle);
				check(ElapsedTime >= 0.0f);
				// ElapsedTime is the time passed in the current loop iteration
				float RemainingDelay = TimePerTick - ElapsedTime;

				ConfigureTickTimer(Handle, ApplicationData->TickTimerHandle, &RemainingDelay);
			}

			// Reset duration timer if needed
			if (!bLoop)
			{
				// Resets previous handles
				if (StackDurationPolicy == EStackDurationPolicy::RESETS)
				{
					ConfigureDeactivationTimer(PrevHandle, PreviousApplicationData->DeactivationTimerHandle, nullptr);
				}
				
				// Syncs to previous handles
				if (!bLocalStackHandled && StackDurationPolicy == EStackDurationPolicy::SYNCRONIZE)
				{
					FTimerManager& TimerManager = Handle.Payload.Target->GetWorldTimerManager();
					const float ElapsedTime = TimerManager.GetTimerElapsed(PreviousApplicationData->DeactivationTimerHandle);
					check(ElapsedTime >= 0.0f);
					float RemainingDuration = Duration - ElapsedTime;

					ConfigureDeactivationTimer(Handle, ApplicationData->DeactivationTimerHandle, &RemainingDuration);
				}
			}

			bLocalStackHandled = true;
		}

		// If we have passed the max stack count, force this application 
		// to be disposed after reseting timers if needed
		const uint32 TotalStackCount = ActiveApplicationHandles.Num() + 1;
		if (TotalStackCount > MaxStackCount)
		{
			SubmitDeactivation(Handle);
		}
	}
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

	if (ApplicationData->DeactivationTimerHandle.IsValid())
	{
		Handle.Payload.Target->GetWorldTimerManager().ClearTimer(ApplicationData->DeactivationTimerHandle);
	}
}
