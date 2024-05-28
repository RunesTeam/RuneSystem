#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettings.h"
#include "InputBindingsDeveloperSettings.generated.h"

class UInputAction;

USTRUCT()
struct FInputActionWrapper
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere)
	UInputAction* inputAction;
};

UCLASS(config = Game, defaultconfig, meta = (DisplayName = "Enhanced Input Settings"))
class RUNESYSTEMSANDBOX_API UInputBindingsDeveloperSettings : public UDeveloperSettings
{
	GENERATED_BODY()
public:
	UInputBindingsDeveloperSettings();

	UPROPERTY(Config, EditAnywhere, Export, Category = "Enhanced Input Configuration|Input Bindings", meta = (ShowOnlyInnerProperties))
		TMap<FName, FInputActionWrapper> namedInputActions;

	const UInputAction* GetInputActionByName(FName name) const;

	UFUNCTION(BlueprintPure)
		static const UInputBindingsDeveloperSettings* GetInputBindingsDeveloperSettings();

};
