


#include "RuneBehaviour.h"
#include "RuneCompatible.h"
#include "RuneTangibleAgent.h"
#include "Utils/RuneUtils.h"


URuneBehaviour::URuneBehaviour() :
	runeOwner(nullptr),
	isPreviewShowing(false)
{
	// Set this component to be initialized when the game starts, and to be ticked every frame.  You can turn these features
	// off to improve performance if you don't need them.
	PrimaryComponentTick.bCanEverTick = true;
}

void URuneBehaviour::BeginPlay()
{
	Super::BeginPlay();
}

void URuneBehaviour::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	if (isPreviewShowing)
	{
		TickPreview(DeltaTime);
	}
}

void URuneBehaviour::ActivateBehaviour()
{
	// by default calls the blueprint version
	ReceiveActivateBehaviour();
}

void URuneBehaviour::DeactivateBehaviour()
{
	// by default calls the blueprint version
	ReceiveDeactivateBehaviour();
}

void URuneBehaviour::ResetBehaviour()
{
	// by default calls the blueprint version
	ReceiveResetBehaviour();
}

bool URuneBehaviour::IsBehaviourActive() const
{
	// by default calls the blueprint version
	return ReceiveIsBehaviourActive();
}

void URuneBehaviour::ShowPreview()
{
	// by default calls the blueprint version
	ReceiveShowPreview();
}

void URuneBehaviour::HidePreview()
{
	// by default calls the blueprint version
	ReceiveHidePreview();
}

void URuneBehaviour::TickPreview(float deltaTime)
{
	// by default calls the blueprint version
	ReceiveTickPreview(deltaTime);
}

bool URuneBehaviour::IsPreviewShowing() const
{
	return isPreviewShowing;
}

ARuneTangibleAgent* URuneBehaviour::SpawnTangibleAgent(UClass* inClass, const FTransform& transform)
{
	//ASSERT(inClass != nullptr, "Tangible agent class has not been properly set");
	return URuneUtils::SpawnTangibleAgent<ARuneTangibleAgent, const FTransform&>(*this, inClass, transform);
}

ARuneTangibleAgent* URuneBehaviour::SpawnTangibleAgent(UClass* inClass, const FVector& location, const FRotator& rotation, const FVector& scale)
{
	return SpawnTangibleAgent(inClass, FTransform(rotation, location, scale));
}

ARuneTangibleAgent* URuneBehaviour::SpawnTangibleAgent(const FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform)
{
	return SpawnTangibleAgentWithTemplate(agentTemplate, transform);
}

ARuneTangibleAgent* URuneBehaviour::SpawnTangibleAgent(const FRuneTangibleAgentTemplate& agentTemplate, const FVector& location, const FRotator& rotation, const FVector& scale)
{
	return SpawnTangibleAgentWithTemplate(agentTemplate, FTransform(rotation, location, scale));
}

ARuneTangibleAgent* URuneBehaviour::SpawnTangibleAgentWithTemplate(const FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform)
{
	//ASSERT(agentTemplate.agentClass != nullptr, "Tangible agent template class has not been properly set");
	return URuneUtils::SpawnTangibleAgent<ARuneTangibleAgent, const FTransform&>(*this, agentTemplate, transform);
}

ARunePreviewAgent* URuneBehaviour::SpawnPreviewAgent(UClass* inClass, const FTransform& transform)
{
	//ASSERT(inClass != nullptr, "Preview agent class has not been properly set");
	return URuneUtils::SpawnPreviewAgent<ARunePreviewAgent, const FTransform&>(*this, inClass, transform);
}

ARunePreviewAgent* URuneBehaviour::SpawnPreviewAgent(UClass* inClass, const FVector& location, const FRotator& rotation, const FVector& scale)
{
	return SpawnPreviewAgent(inClass, FTransform(rotation, location, scale));
}

ARunePreviewAgent* URuneBehaviour::SpawnPreviewAgent(const FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform)
{
	return SpawnPreviewAgentWithTemplate(agentTemplate, transform);
}

ARunePreviewAgent* URuneBehaviour::SpawnPreviewAgent(const FRuneTangibleAgentTemplate& agentTemplate, const FVector& location, const FRotator& rotation, const FVector& scale)
{
	return SpawnPreviewAgentWithTemplate(agentTemplate, FTransform(rotation, location, scale));
}

ARunePreviewAgent* URuneBehaviour::SpawnPreviewAgentWithTemplate(const FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform)
{
	//ASSERT(agentTemplate.agentClass != nullptr, "Preview agent template class has not been properly set");
	return URuneUtils::SpawnPreviewAgent<ARunePreviewAgent, const FTransform&>(*this, agentTemplate, transform);
}

bool URuneBehaviour::BroadcastApplyPulse(AActor* actor) const
{
	bool success = false;
	FBooleanPtr successPtr({ &success });

	if (onApplyPulse.IsBound() && runeOwner != nullptr)
	{
		onApplyPulse.Broadcast(runeOwner->GetController(), (AActor*) runeOwner->GetController(), actor, successPtr);
	}
	onApplyPulseBroadcast.Broadcast(actor, success);

	return success;
}

bool URuneBehaviour::BroadcastRevertPulse(AActor* actor) const
{
	bool success = false;
	FBooleanPtr successPtr({ &success });

	if (onRevertPulse.IsBound())
	{
		onRevertPulse.Broadcast(actor, successPtr);
	}
	onRevertPulseBroadcast.Broadcast(actor, success);

	return success;
}

void URuneBehaviour::InternalActivateBehaviour()
{
	ActivateBehaviour();
	onBehaviourActivate.Broadcast();
}

void URuneBehaviour::InternalDeactivateBehaviour()
{
	DeactivateBehaviour();
	onBehaviourDeactivate.Broadcast();
}

void URuneBehaviour::InternalResetBehaviour()
{
	ResetBehaviour();
	onBehaviourReset.Broadcast();
}

bool URuneBehaviour::InternalShowPreview()
{
	if (!isPreviewShowing)
	{
		onShowPreviewBegin.Broadcast();
		ShowPreview();
		isPreviewShowing = true;
		onShowPreviewEnd.Broadcast();

		return true;
	}

	return false;
}

bool URuneBehaviour::InternalHidePreview()
{
	if (isPreviewShowing)
	{
		onHidePreviewBegin.Broadcast();
		HidePreview();
		isPreviewShowing = false;
		onHidePreviewEnd.Broadcast();

		return true;
	}

	return false;
}
