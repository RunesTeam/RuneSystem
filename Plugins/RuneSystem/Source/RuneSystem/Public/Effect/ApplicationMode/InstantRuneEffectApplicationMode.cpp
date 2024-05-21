


#include "InstantRuneEffectApplicationMode.h"
#include "Effect/RuneEffect.h"
#include "Effect/RuneEffectHandle.h"


UInstantRuneEffectApplicationMode::UInstantRuneEffectApplicationMode()
{
}

void UInstantRuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	Super::HandleEffectActivation(Handle);

	check(IsValid(Handle.Payload.Target));
	SubmitApply(Handle);
	SubmitDeactivation(Handle);
}

void UInstantRuneEffectApplicationMode::HandleEffectDeactivation(const FRuneEffectHandle& Handle)
{
	Super::HandleEffectDeactivation(Handle);

	check(IsValid(Handle.Payload.Target));
	SubmitRevert(Handle);
}
