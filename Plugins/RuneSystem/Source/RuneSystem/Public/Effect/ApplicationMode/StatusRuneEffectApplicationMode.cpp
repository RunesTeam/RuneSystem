


#include "StatusRuneEffectApplicationMode.h"
#include "RuneSystem.h"
#include "Effect/RuneEffect.h"
#include "Effect/RuneEffectHandle.h"


UStatusRuneEffectApplicationMode::UStatusRuneEffectApplicationMode() :
	Duration(1.0f),
	StackDurationPolicy(EStackDurationPolicy::NONE),
	MaxStackCount(0u)
{
}

void UStatusRuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	auto ConfigureDeactivationTimer = [this](const FRuneEffectHandle& InHandle, FTimerHandle& TimerHandle, float* InDuration)
		{
			FTimerManager& TimerManager = InHandle.Payload.Target->GetWorldTimerManager();
			TimerManager.SetTimer(
				TimerHandle,
				FTimerDelegate::CreateLambda([=, this]() mutable
					{
						SubmitDeactivation(InHandle);
					}),
				InDuration != nullptr ? *InDuration : Duration,
				false);
		};

	Super::HandleEffectActivation(Handle);

	check(IsValid(Handle.Payload.Target));

	SubmitApply(Handle);

	// If duration is set to a negative number the number of tick will be indefinite
	// Auxiliary variables initialization
	const bool bIsInfinite = Duration < 0.0f;
	UStatusRuneEffectApplicationData* ApplicationData = NewObject<UStatusRuneEffectApplicationData>();
	check(IsValid(ApplicationData));
	if (!bIsInfinite)
	{
		ConfigureDeactivationTimer(Handle, ApplicationData->TimerHandle, nullptr);
	}
	URuneSystem::SetEffectHandleApplicationData(this, Handle, ApplicationData);

	// Handle stack application
	const bool bCanBeStacked = StackDurationPolicy != EStackDurationPolicy::NONE;
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
			UStatusRuneEffectApplicationData* PreviousApplicationData = Cast<UStatusRuneEffectApplicationData>(URuneSystem::GetEffectHandleApplicationData(this, PrevHandle));
			check(IsValid(PreviousApplicationData));

			// Reset duration timer if needed
			if (!bIsInfinite)
			{
				// Resets previous handles
				if (StackDurationPolicy == EStackDurationPolicy::RESETS)
				{
					ConfigureDeactivationTimer(PrevHandle, PreviousApplicationData->TimerHandle, nullptr);
				}

				// Syncs to previous handles
				if (!bLocalStackHandled && StackDurationPolicy == EStackDurationPolicy::SYNCRONIZE)
				{
					FTimerManager& TimerManager = Handle.Payload.Target->GetWorldTimerManager();
					const float ElapsedTime = TimerManager.GetTimerElapsed(PreviousApplicationData->TimerHandle);
					check(ElapsedTime >= 0.0f);
					float RemainingDuration = Duration - ElapsedTime;

					ConfigureDeactivationTimer(Handle, ApplicationData->TimerHandle, &RemainingDuration);
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

void UStatusRuneEffectApplicationMode::HandleEffectDeactivation(const FRuneEffectHandle& Handle)
{
	Super::HandleEffectDeactivation(Handle);

	SubmitRevert(Handle);

	UStatusRuneEffectApplicationData* ApplicationData = Cast<UStatusRuneEffectApplicationData>(URuneSystem::GetEffectHandleApplicationData(this, Handle));
	if (IsValid(ApplicationData) && ApplicationData->TimerHandle.IsValid())
	{
		Handle.Payload.Target->GetWorldTimerManager().ClearTimer(ApplicationData->TimerHandle);
	}
}
