


#include "InstantRuneEffectApplicationMode.h"
#include "RuneEffect.h"


UInstantRuneEffectApplicationMode::UInstantRuneEffectApplicationMode()
{
}

void UInstantRuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	check(IsValid(Handle.Payload.Target));
	SubmitApply(Handle);
	SubmitDeactivation(Handle);
}

void UInstantRuneEffectApplicationMode::HandleEffectDeactivation(const FRuneEffectHandle& Handle)
{
	check(IsValid(Handle.Payload.Target));
	SubmitRevert(Handle);
}
