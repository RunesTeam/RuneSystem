


#include "StatusRuneEffectApplicationMode.h"
#include "RuneEffect.h"
#include "RuneSystem.h"


UStatusRuneEffectApplicationMode::UStatusRuneEffectApplicationMode() :
	Duration(1.0f)
{
}

void UStatusRuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	check(IsValid(Handle.Payload.Target));

	SubmitApply(Handle);

	// auxiliary variables initialization
	// If duration is set to a negative number the number of tick will be indefinite
	const bool IsInfinite = Duration < 0.0f;
	if (!IsInfinite)
	{
		UStatusRuneEffectApplicationData* ApplicationData = NewObject<UStatusRuneEffectApplicationData>();
		check(IsValid(ApplicationData));

		Handle.Payload.Target->GetWorldTimerManager().SetTimer(
			ApplicationData->TimerHandle,
			FTimerDelegate::CreateLambda([=, this]()
				{
					SubmitDeactivation(Handle);
				}),
			Duration,
			false);

		URuneSystem::SetEffectHandleApplicationData(Handle, ApplicationData);
	}
}

void UStatusRuneEffectApplicationMode::HandleEffectDeactivation(const FRuneEffectHandle& Handle)
{
	SubmitRevert(Handle);

	UStatusRuneEffectApplicationData* ApplicationData = Cast<UStatusRuneEffectApplicationData>(URuneSystem::GetEffectHandleApplicationData(Handle));
	if (IsValid(ApplicationData) && ApplicationData->TimerHandle.IsValid())
	{
		Handle.Payload.Target->GetWorldTimerManager().ClearTimer(ApplicationData->TimerHandle);
	}
}
