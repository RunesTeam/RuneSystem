

#pragma once

#include "CoreMinimal.h"
#include "Factories/Factory.h"
#include "RuneFilterFactory.generated.h"

/**
 * 
 */
UCLASS()
class URuneFilterFactory : public UFactory
{
	GENERATED_BODY()

public:
	URuneFilterFactory();

protected:
	virtual UObject* FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn) override;

};
