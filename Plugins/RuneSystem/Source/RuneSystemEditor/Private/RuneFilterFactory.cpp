


#include "RuneFilterFactory.h"
#include "RuneFilter.h"


URuneFilterFactory::URuneFilterFactory()
{
    SupportedClass = URuneFilter::StaticClass();
	bCreateNew = true;
}

UObject* URuneFilterFactory::FactoryCreateNew(UClass* InClass, UObject* InParent, FName InName, EObjectFlags Flags, UObject* Context, FFeedbackContext* Warn)
{
    return NewObject<URuneFilter>(InParent, InClass, InName, Flags);
}
