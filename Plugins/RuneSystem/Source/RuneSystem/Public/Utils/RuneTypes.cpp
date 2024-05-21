


#include "RuneTypes.h"
#include "RuneTangibleAgent.h"


UClass* URuneBlueprintFunctionLibrary::Conv_RuneTangibleAgentTemplateToClass(const FRuneTangibleAgentTemplate& inTemplate)
{
	return inTemplate.agentClass;
}
