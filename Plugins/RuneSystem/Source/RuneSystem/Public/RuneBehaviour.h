

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Utils/RuneTypes.h"
#include "RuneBehaviour.generated.h"


class IRuneCompatible;
class ARuneTangibleAgent;
class ARunePreviewAgent;

DECLARE_DYNAMIC_MULTICAST_DELEGATE_FourParams(FRunePulseDelegate, AController*, instigator, AActor*, causer, AActor*, target, FBooleanPtr, success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRuneRevertDelegate, AActor*, target, FBooleanPtr, success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRuneBehaviourActivationDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FRuneBehaviourApplicationDelegate, AActor*, target, bool, success);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRunePreviewDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRuneTangibleAgentDelegate, ARuneTangibleAgent*, agent);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRunePreviewAgentDelegate, ARunePreviewAgent*, previewAgent);


UCLASS(Abstract, Blueprintable, EditInlineNew, HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking, "Components|Activation"))
class RUNESYSTEM_API URuneBehaviour : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URuneBehaviour();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void TickComponent(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

public:
	/**
	 * Manages the activation of the behaviour.
	 * (e.g. spawn a ARuneTangibleAgent)
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ActivateBehaviour();

	/**
	 * Manages the deactivation of the behaviour.
	 * (e.g. destroy a ARuneTangibleAgent)
	 */
	UFUNCTION(BlueprintCallable)
	virtual void DeactivateBehaviour();

	/**
	 * Resets the state of the behaviour that gets modified
	 * via the owner's input.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ResetBehaviour();

	/**
	 * Checks whether the behaviour is still active.
	 * (e.g. whether a ARuneTangibleAgent is still alive)
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool IsBehaviourActive() const;

	/**
	 * Shows the behaviour's visual preview.
	 * (e.g. show skill shot indicator)
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ShowPreview();

	/**
	 * Hides the behaviour's visual preview.
	 * (e.g. hide skill shot indicator)
	 */
	UFUNCTION(BlueprintCallable)
	virtual void HidePreview();

	/**
	 * Manages the behaviour's visual preview.
	 * (e.g. moves skill shot indicator)
	 * 
	 * @param deltaTime Delta time in seconds
	 */
	UFUNCTION(BlueprintCallable)
	virtual void TickPreview(float deltaTime);

	/**
	 * Whether or not the preview is currently showing.
	 * 
	 * @return If true, preview is showing.
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool IsPreviewShowing() const;

protected:
	/**
	 * Spawn and configures (attaches RuneEffects) a RuneTangibleAgent.
	 *
	 * @param inClass Spawned class
	 * @param transform Transform of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	UFUNCTION(BlueprintCallable)
	ARuneTangibleAgent* SpawnTangibleAgent(UClass* inClass, const FTransform& transform);

	/**
	 * Spawn and configures (attaches RuneEffects) a RuneTangibleAgent.
	 *
	 * @param inClass Spawned class
	 * @param location Location of the new spawned agent.
	 * @param rotation Rotation of the new spawned agent.
	 * @param scale Scale of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	ARuneTangibleAgent* SpawnTangibleAgent(UClass* inClass, const FVector& location = FVector::ZeroVector, const FRotator& rotation = FRotator::ZeroRotator, const FVector& scale = FVector::OneVector);

	/**
	 * Spawn and configures (attaches RuneEffects) a RuneTangibleAgent.
	 *
	 * @param agentTemplate Template containing the spawned class
	 * @param transform Transform of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	ARuneTangibleAgent* SpawnTangibleAgent(const struct FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform);

	/**
	 * Spawn and configures (attaches RuneEffects) a RuneTangibleAgent.
	 *
	 * @param agentTemplate Template containing the spawned class
	 * @param location Location of the new spawned agent.
	 * @param rotation Rotation of the new spawned agent.
	 * @param scale Scale of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	ARuneTangibleAgent* SpawnTangibleAgent(const struct FRuneTangibleAgentTemplate& agentTemplate, const FVector& location = FVector::ZeroVector, const FRotator& rotation = FRotator::ZeroRotator, const FVector& scale = FVector::OneVector);

	/**
	 * Spawn and configures (attaches RuneEffects) a RuneTangibleAgent via a template.
	 *
	 * @param agentTemplate Template containing the spawned class
	 * @param transform Transform of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	UFUNCTION(BlueprintCallable)
	ARuneTangibleAgent* SpawnTangibleAgentWithTemplate(const struct FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform);

	/**
	 * Spawn and configures (attaches RuneEffects) a RunePreviewAgent.
	 *
	 * @param inClass Spawned class
	 * @param transform Transform of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	UFUNCTION(BlueprintCallable)
	ARunePreviewAgent* SpawnPreviewAgent(UClass* inClass, const FTransform& transform);

	/**
	 * Spawn a RunePreviewAgent.
	 *
	 * @param inClass Spawned class
	 * @param location Location of the new spawned agent.
	 * @param rotation Rotation of the new spawned agent.
	 * @param scale Scale of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	ARunePreviewAgent* SpawnPreviewAgent(UClass* inClass, const FVector& location = FVector::ZeroVector, const FRotator& rotation = FRotator::ZeroRotator, const FVector& scale = FVector::OneVector);

	/**
	 * Spawn a RunePreviewAgent.
	 *
	 * @param agentTemplate Template containing the spawned class
	 * @param transform Transform of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	ARunePreviewAgent* SpawnPreviewAgent(const struct FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform);

	/**
	 * Spawn a RunePreviewAgent.
	 *
	 * @param agentTemplate Template containing the spawned class
	 * @param location Location of the new spawned agent.
	 * @param rotation Rotation of the new spawned agent.
	 * @param scale Scale of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	ARunePreviewAgent* SpawnPreviewAgent(const struct FRuneTangibleAgentTemplate& agentTemplate, const FVector& location = FVector::ZeroVector, const FRotator& rotation = FRotator::ZeroRotator, const FVector& scale = FVector::OneVector);

	/**
	 * Spawn and configures (attaches RuneEffects) a RunePreviewAgent via a template.
	 *
	 * @param agentTemplate Template containing the spawned class
	 * @param transform Transform of the new spawned agent.
	 * @return Pointer to the spawned agent. If nullptr, agent could not be spawned.
	 */
	UFUNCTION(BlueprintCallable)
	ARunePreviewAgent* SpawnPreviewAgentWithTemplate(const struct FRuneTangibleAgentTemplate& agentTemplate, const FTransform& transform);

	/**
	 * Broadcast an apply pulse (signal) for those effects
	 * binded to the OnApplyPulse delegate.
	 *
	 * @param actor Actor affected by the pulse
	 * @return If true, at least one effect have been applied succesfully
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = "false")
	bool BroadcastApplyPulse(AActor* actor) const;

	/**
	 * Broadcast a revert pulse (signal) for those effects
	 * binded to the OnRevertPulse delegate.
	 *
	 * @param actor Actor affected by the pulse
	 * @return If true, at least one effect have been applied succesfully
	 */
	UFUNCTION(BlueprintCallable, BlueprintPure = "false")
	bool BroadcastRevertPulse(AActor* actor) const;

	/**
	 * Blueprint version of ActivateBehaviour() method.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Activate Behaviour"))
	void ReceiveActivateBehaviour();

	/**
	 * Blueprint version of DeactivateBehaviour() method.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Deactivate Behaviour"))
	void ReceiveDeactivateBehaviour();

	/**
	 * Blueprint version of ResetBehaviour() method.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Reset Behaviour"))
	void ReceiveResetBehaviour();

	/**
	 * Blueprint version of IsBehaviourActive() method.
	 */
	UFUNCTION(BlueprintImplementableEvent, BlueprintPure, meta = (ForceAsFunction, DisplayName = "Is Behaviour Active"))
	bool ReceiveIsBehaviourActive() const;

	/**
	 * Blueprint version of ShowPreview() method.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction, DisplayName = "Show Preview"))
	void ReceiveShowPreview();

	/**
	 * Blueprint version of HidePreview() method.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction, DisplayName = "Hide Preview"))
	void ReceiveHidePreview();

	/**
	 * Blueprint version of TickPreview(float) method.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (ForceAsFunction, DisplayName = "Tick Preview"))
	void ReceiveTickPreview(float DeltaTime);

private:
	/** Intermediate ActivateBehaviour() method used for delegate broadcasting coherence */
	virtual void InternalActivateBehaviour();

	/** Intermediate DeactivateBehaviour() method used for delegate broadcasting coherence */
	virtual void InternalDeactivateBehaviour();

	/** Intermediate ResetBehaviour() method used for delegate broadcasting coherence */
	virtual void InternalResetBehaviour();

	/** Intermediate ShowPreview() method used for delegate broadcasting coherence */
	virtual bool InternalShowPreview();

	/** Intermediate ShowPreview() method used for delegate broadcasting coherence */
	virtual bool InternalHidePreview();

public:
	/** Called when a behaviour should send an apply pulse to an actor */
	UPROPERTY()
	FRunePulseDelegate onApplyPulse;

	/** Called when a behaviour should send a revert pulse to an actor */
	UPROPERTY()
	FRuneRevertDelegate onRevertPulse;

	/** Called when a behaviour has been activated */
	UPROPERTY(BlueprintAssignable)
	FRuneBehaviourActivationDelegate onBehaviourActivate;
	
	/** Called when a behaviour has been deactivated */
	UPROPERTY(BlueprintAssignable)
	FRuneBehaviourActivationDelegate onBehaviourDeactivate;

	/** Called when a behaviour has been reset */
	UPROPERTY(BlueprintAssignable)
	FRuneBehaviourActivationDelegate onBehaviourReset;

	/** Called when a behaviour has sent an apply pulse */
	UPROPERTY(BlueprintAssignable)
	FRuneBehaviourApplicationDelegate onApplyPulseBroadcast;

	/** Called when a behaviour has sent an revert pulse */
	UPROPERTY(BlueprintAssignable)
	FRuneBehaviourApplicationDelegate onRevertPulseBroadcast;

	/** Called when a behaviour has began spawning a tangible agent (not placed in world yet) */
	UPROPERTY(BlueprintAssignable)
	FRuneTangibleAgentDelegate onTangibleAgentSpawnBegin;

	/** Called when a behaviour has ended spawning a tangible agent (just placed in world) */
	UPROPERTY(BlueprintAssignable)
	FRuneTangibleAgentDelegate onTangibleAgentSpawnEnd;

	/** Called when a behaviour has began spawning a preview agent (not placed in world yet) */
	UPROPERTY(BlueprintAssignable)
	FRunePreviewAgentDelegate onPreviewAgentSpawnBegin;

	/** Called when a behaviour has ended spawning a preview agent (just placed in world) */
	UPROPERTY(BlueprintAssignable)
	FRunePreviewAgentDelegate onPreviewAgentSpawnEnd;

	/** Called when a preview is about to start showing */
	UPROPERTY(BlueprintAssignable)
	FRunePreviewDelegate onShowPreviewBegin;

	/** Called when a preview has ended showing */
	UPROPERTY(BlueprintAssignable)
	FRunePreviewDelegate onShowPreviewEnd;

	/** Called when a preview is about to start hiding */
	UPROPERTY(BlueprintAssignable)
	FRunePreviewDelegate onHidePreviewBegin;

	/** Called when a preview has ended hiding */
	UPROPERTY(BlueprintAssignable)
	FRunePreviewDelegate onHidePreviewEnd;

protected:
	friend class URuneBaseComponent;
	friend class URuneCastStateMachine;
	friend class URuneUtils;

	/** Cached owner. It could be nullptr. */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RuneBehaviour: Debug Variables")
	TScriptInterface<IRuneCompatible> runeOwner;

private:
	bool isPreviewShowing;

};
