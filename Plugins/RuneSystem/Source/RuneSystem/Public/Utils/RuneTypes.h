

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "RuneTypes.generated.h"


USTRUCT(BlueprintType)
struct FRuneTangibleAgentTemplate
{
	GENERATED_BODY()

public:
	FRuneTangibleAgentTemplate() = default;

	operator TSubclassOf<class ARuneTangibleAgent>() const
	{
		return agentClass;
	};

public:
	/** Sets the class of TangibleAgent to be used */
	UPROPERTY(EditAnywhere)
	TSubclassOf<class ARuneTangibleAgent> agentClass = nullptr;

#if WITH_EDITORONLY_DATA
	/** Editable instance of the selected TangibleAgent class */
	UPROPERTY(VisibleAnywhere, Instanced, Export, Transient, meta = (EditCondition = "agentClass!=nullptr", EditConditionHides))
	class ARuneTangibleAgent* agentTemplateObject = nullptr;
#endif

	UPROPERTY(VisibleAnywhere, Export)
	TMap<FName, FString> properties;
};


UCLASS()
class URuneBlueprintFunctionLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	/**
	* Function to auto-convert TangibleAgentTemplate to TangibleAgent class.
	* @param inTemplate The template to be converted
	* @return The UClass of the template
	*/
	UFUNCTION(BlueprintPure, meta = (DisplayName = "Tangible Agent Template To Tangible Agent Class", CompactNodeTitle = "->", BlueprintAutocast), Category = "RuneSystem|TangibleAgent")
	static UClass* Conv_RuneTangibleAgentTemplateToClass(const FRuneTangibleAgentTemplate& inTemplate);

};
