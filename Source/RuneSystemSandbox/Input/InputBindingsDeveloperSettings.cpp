#include "Input/InputBindingsDeveloperSettings.h"

UInputBindingsDeveloperSettings::UInputBindingsDeveloperSettings()
{
}

const UInputAction* UInputBindingsDeveloperSettings::GetInputActionByName(FName name) const
{
	FInputActionWrapper const* action = namedInputActions.Find(name);
	if (action != nullptr)
	{
		return action->inputAction;
	}
	else
	{
		return nullptr;
	}
}

const UInputBindingsDeveloperSettings* UInputBindingsDeveloperSettings::GetInputBindingsDeveloperSettings()
{
	return GetDefault<UInputBindingsDeveloperSettings>();
}
