


#include "RuneUtils.h"
#include "RuneEffect.h"


bool URuneUtils::ApplyEffect(URuneEffect* effect, AController* instigator, AActor* causer, AActor* target)
{
	if (effect == nullptr || target == nullptr) return false;

	bool success = false;
	effect->InternalApply(instigator, causer, target, { &success });
	return success;
}

bool URuneUtils::RevertEffect(URuneEffect* effect, AActor* target)
{
	if (effect == nullptr || target == nullptr) return false;

	bool success = false;
	effect->InternalRevert(target, { &success });
	return success;
}

