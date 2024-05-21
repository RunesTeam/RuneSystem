


#include "StatusRuneEffectApplicationMode.h"
#include "RuneSystem.h"
#include "Effect/RuneEffect.h"
#include "Effect/RuneEffectHandle.h"


UStatusRuneEffectApplicationMode::UStatusRuneEffectApplicationMode() :
	Duration(1.0f)
{
}

void UStatusRuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	Super::HandleEffectActivation(Handle);

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

		URuneSystem::SetEffectHandleApplicationData(this, Handle, ApplicationData);
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
