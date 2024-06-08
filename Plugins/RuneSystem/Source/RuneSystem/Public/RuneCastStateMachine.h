

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RuneCastStateMachine.generated.h"

UENUM()
enum class ETransitionPolicy
{
	/** Transition instantly when calling ChangeState */
	INSTANT = 0,

	/** Transition in the beginning of the next frame */
	NEXT_TICK,

	/** Transition in the end of the current frame */
	END_TICK
};


DECLARE_DYNAMIC_MULTICAST_DELEGATE(FRuneCastDelegate);
DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FRuneCastStateDelegate, const UState*, state);

DECLARE_DYNAMIC_MULTICAST_DELEGATE_OneParam(FStateTickDelegate, float, deltaTime);
DECLARE_DYNAMIC_MULTICAST_DELEGATE(FStateTransitionDelegate);

class URuneBehaviour;

UCLASS(BlueprintType, Blueprintable)
class RUNESYSTEM_API UState : public UObject
{
	GENERATED_BODY()

public:
	// Sets default values for this object's properties
	UState() = default;

public:
	UPROPERTY(EditAnywhere, BlueprintReadOnly)
	FName nameID = NAME_None;

	/** Delegate invoked when a state ticks. */
	UPROPERTY(BlueprintAssignable)
	FStateTickDelegate onTick;

	/** Delegate invoked just when entered the state. */
	UPROPERTY(BlueprintAssignable)
	FStateTransitionDelegate onEnter;

	/** Delegate invoked just when exited the state. */
	UPROPERTY(BlueprintAssignable)
	FStateTransitionDelegate onExit;
};

UCLASS(Abstract, ClassGroup = "RuneCast", Blueprintable, EditInlineNew, AutoExpandCategories = "RuneCastStateMachine: General Settings", HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking, "Components|Activation"))
class RUNESYSTEM_API URuneCastStateMachine : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	URuneCastStateMachine();

#if WITH_EDITOR
	/**
	 * Called by the editor to query whether a property of this object is allowed to be modified.
	 * The property editor uses this to disable controls for properties that should not be changed.
	 * When overriding this function you should always call the parent implementation first.
	 *
	 * @param InProperty The property to query
	 *
	 * @return true if the property can be modified in the editor, otherwise false
	 */
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:

	virtual void TickCastStateMachine(float DeltaTime, enum ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction);

public:
	/**
	 * Method invoked in BeginPlay(), you can either override this method or use the BeginPlay() method.
	 * Use it to configure the states functionality this StateMachine will manage.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void Init();

	/**
	 * Sets the entry state. 
	 * Should be called in BeginPlay() or right after Init().
	 * This state will be the first state that will tick
	 * when the state machine starts running.
	 * 
	 * @param state The entry state
	 */
	UFUNCTION(BlueprintCallable)
	virtual void SetEntryState(UState* state);

	/**
	 * Sets new value for state and calls corresponding OnEnter/OnExit delegates.
	 *
	 * @param state New state for the state machine
	 */
	UFUNCTION(BlueprintCallable)
	virtual void ChangeState(const UState* state);

	/**
	 * Sets IsPressed to true.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void SetPressed();

	/**
	 * Sets IsPressed to false.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void SetReleased();

	/**
	 * Resumes the execution of the state machine.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void Resume();

	/**
	 * Pauses the execution of the state machine.
	 */
	UFUNCTION(BlueprintCallable)
	virtual void Pause();

	/**
	 * Whether the state machine is running or paused.
	 * 
	 * @return If true, it is running. If false, it is paused
	 */
	UFUNCTION(BlueprintCallable)
	virtual bool IsRunning() const;

	/**
	 * Sets the behaviour controlled by this state machine.
	 *
	 * @param behaviour Linked RuneBehaviour.
	 */
	UFUNCTION(BlueprintCallable)
	void SetLinkedBehaviour(TArray<URuneBehaviour*> runeBehaviours);

	/**
	 * Creates a new State and adds it the the states list.
	 * Sets a given name as an identifier. Unique names are recommended.
	 * 
	 * @param name Name that identifies the state.
	 * @return Newly created state.
	 */
	UFUNCTION(BlueprintCallable)
	UState* CreateState(FName name = NAME_None);

	/**
	 * Destroys a state and removes it from the states.
	 *
	 * @param state State that should be destroyed
	 */
	UFUNCTION(BlueprintCallable)
	void DestroyState(UState* state);

	/**
	 * Retrieves a state by its name.
	 * 
	 * @param name Name of the state
	 * @return State
	 */
	UFUNCTION(BlueprintCallable)
	UState* GetState(FName name) const;

	/**
	 * Retrieves all states with a given name.
	 *
	 * @param name Name of the states
	 * @return Array of states
	 */
	UFUNCTION(BlueprintCallable)
	TArray<UState*> GetStatesByName(FName name) const;

	/**
	 * Retrieves all states.
	 *
	 * @return Array of states
	 */
	UFUNCTION(BlueprintCallable)
	const TArray<UState*>& GetStates() const;

	/**
	 * Retrieves the currently managed state.
	 * 
	 * @return State
	 */
	UFUNCTION(BlueprintCallable)
	UState* GetCurrentState() const;

	/**
	 * Activates a behaviour in a given slot index.
	 * 
	 * @param index Slot index
	 * @return If true, behaviour has successfully been activated
	 */
	UFUNCTION(BlueprintCallable)
	bool ActivateBehaviourSlot(int index = 0);

	/**
	 * Deactivates a behaviour in a given slot index.
	 *
	 * @param index Slot index
	 * @return If true, behaviour has successfully been deactivated
	 */
	UFUNCTION(BlueprintCallable)
	bool DeactivateBehaviourSlot(int index = 0);

	/**
	 * Deactivates a behaviour in a given slot index.
	 *
	 * @param index Slot index
	 * @return If true, behaviour has successfully been reset
	 */
	UFUNCTION(BlueprintCallable)
	bool ResetBehaviourSlot(int index = 0);

	/**
	 * Shows a behaviour preview in a given slot index.
	 *
	 * @param index Slot index
	 * @return If true, behaviour has successfully been shown
	 */
	UFUNCTION(BlueprintCallable)
	bool ShowBehaviourSlotPreview(int index = 0);

	/**
	 * Hides a behaviour preview in a given slot index.
	 *
	 * @param index Slot index
	 * @return If true, behaviour has successfully been hidden
	 */
	UFUNCTION(BlueprintCallable)
	bool HideBehaviourSlotPreview(int index = 0);

	/**
	 * Whether or not a behaviour preview is currently showing.
	 * 
	 * @param index Slot index
	 * @return If true, behaviour preview is showing
	 */
	UFUNCTION(BlueprintCallable)
	bool IsBehaviourSlotPreviewShowing(int index = 0);

	/**
	 * Whether or not a behaviour slot is valid.
	 *
	 * @param index Slot index
	 * @return If true, slot has a valid behaviour.
	 */
	UFUNCTION(BlueprintCallable)
	bool IsBehaviourSlotValid(int index = 0) const;

	/**
	 * Get all behaviour slots.
	 * 
	 * @return TArray containing all behaviour slots.
	 */
	UFUNCTION(BlueprintCallable)
	const TArray<URuneBehaviour*>& GetBehaviourSlots() const;

	/**
	 * Get behaviour in a given slot index.
	 * 
	 * @param index Slot index
	 * @return URuneBehaviour in a given slot. Can be nullptr.
	 */
	UFUNCTION(BlueprintCallable)
	URuneBehaviour* GetBehaviourSlot(int index = 0) const;

	/**
	 * Get behaviour slot count.
	 *
	 * @return Used slot count. Not necessarily equal to slots.
	 */
	UFUNCTION(BlueprintCallable)
	int32 GetBehaviourSlotCount() const;

protected:
	/**
	 * Method invoked in BeginPlay().
	 * Use it to configure the states and the transitions this StateMachine will manage.
	 */
	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Init"))
	void ReceiveInit();
	/**
	 * Checks whether the pending states queue is empty or not. This information
	 * can be useful for when multiple conditions for changing state are met on the
	 * same frame, but only the first one should be accepted
	 * 
	 * @return true if there are any pending states to change to, false otherwise
	 */
	UFUNCTION(BlueprintCallable)
	bool AreAnyStatesEnqueued() { return !_pendingStates.IsEmpty(); };

private:
	/**
	 * Performs the transition to the given state.
	 * Invokes all event delegates (does NOT tick)
	 * 
	 * @param state Transitioned state
	 */
	void PerformTransition(const UState* state);

public:
	/** What policy should be used when changing states */
	UPROPERTY(EditAnywhere, Category = "RuneCastStateMachine: General Settings")
	ETransitionPolicy transitionPolicy;

	/** Whether there are multiple slots */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RuneCastStateMachine: General Settings")
	bool hasMultipleSlots;

	/** Number of different behaviours the machine can activate */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "RuneCastStateMachine: General Settings", meta = (EditCondition = "hasMultipleSlots", EditConditionHides, ClampMin = 1, UIMin = 1))
	int slots;

	/** Whether the slot count is fixed or editable. Used in Editor-only */
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "RuneCastStateMachine: General Settings", meta = (EditCondition = "hasMultipleSlots", EditConditionHides, ClampMin = 1, UIMin = 1))
	bool fixedSlotCount;

	/** Called when the state machine has received a press input */
	UPROPERTY(BlueprintAssignable)
	FRuneCastDelegate onPress;

	/** Called when the state machine has received a release input */
	UPROPERTY(BlueprintAssignable)
	FRuneCastDelegate onRelease;

	/** Called when the state machine has been resumed from a pause state */
	UPROPERTY(BlueprintAssignable)
	FRuneCastDelegate onResume;

	/** Called when the state machine has been paused from a running state */
	UPROPERTY(BlueprintAssignable)
	FRuneCastDelegate onPause;

	/** Called when the state machine has entered a state */
	UPROPERTY(BlueprintAssignable)
	FRuneCastStateDelegate onStateEnter;

	/** Called when the state machine has ticked a state */
	UPROPERTY(BlueprintAssignable)
	FRuneCastStateDelegate onStateTick;

	/** Called when the state machine has exited a state */
	UPROPERTY(BlueprintAssignable)
	FRuneCastStateDelegate onStateExit;

protected:
	/** State that is currently ticking */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RuneCastStateMachine: Debug Variables")
	const UState* currentState;

	/** Cached pressed input */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RuneCastStateMachine: Debug Variables")
	bool isPressed;

private:
	/** Entry state. This is not cached */
	UPROPERTY()
	const UState* _entryState;

	/** Cached states. This will prevent GC from freeing the memory */
	UPROPERTY()
	TArray<UState*> _states;

	/** Pending states that have to be transitioned to */
	TQueue<const UState*> _pendingStates;

	/** Linked RuneBehaviour this StateMachine is managing */
	UPROPERTY(VisibleInstanceOnly, Category = "RuneCastStateMachine: Debug Variables")
	TArray<URuneBehaviour*> runeBehaviours;

};
