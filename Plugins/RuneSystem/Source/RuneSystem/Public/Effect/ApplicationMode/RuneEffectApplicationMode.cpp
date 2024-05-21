

#include "RuneEffectApplicationMode.h"
#include "Effect/RuneEffect.h"
#include "Effect/RuneEffectHandle.h"


void URuneEffectApplicationMode::HandleEffectActivation(const FRuneEffectHandle& Handle)
{
	// by default calls the blueprint version
	ReceiveHandleEffectActivation(Handle);
}

void URuneEffectApplicationMode::HandleEffectDeactivation(const FRuneEffectHandle& Handle)
{
	// by default calls the blueprint version
	ReceiveHandleEffectDeactivation(Handle);
}

void URuneEffectApplicationMode::SubmitApply(const FRuneEffectHandle& Handle)
{
	check(IsValid(Handle.Effect));
	Handle.Effect->SubmitApply(Handle.Payload);
}

void URuneEffectApplicationMode::SubmitRevert(const FRuneEffectHandle& Handle)
{
	check(IsValid(Handle.Effect));
	Handle.Effect->SubmitRevert(Handle.Payload);
}

void URuneEffectApplicationMode::SubmitActivation(const FRuneEffectHandle& Handle)
{
	check(IsValid(Handle.Effect));
	Handle.Effect->Activate(Handle.Payload);
}

void URuneEffectApplicationMode::SubmitDeactivation(const FRuneEffectHandle& Handle)
{
	check(IsValid(Handle.Effect));
	Handle.Effect->Deactivate(Handle);
}
