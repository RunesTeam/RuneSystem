


#include "RuneCastStateMachine.h"
#include "RuneBehaviour.h"


URuneCastStateMachine::URuneCastStateMachine() : 
	transitionPolicy(ETransitionPolicy::END_TICK),
	hasMultipleSlots(false),
	slots(1),
	fixedSlotCount(false),
	currentState(nullptr),
	isPressed(false),
	_entryState(nullptr),
	_states(),
	_pendingStates(),
	runeBehaviours()
{
	// should tick to update the owned states
	PrimaryComponentTick.bCanEverTick = true;
}

void URuneCastStateMachine::BeginPlay()
{
	Super::BeginPlay();

	// initialize states and its transitions
	Init();
}

#if WITH_EDITOR
bool URuneCastStateMachine::CanEditChange(const FProperty* InProperty) const
{
	bool result = Super::CanEditChange(InProperty);

	bool isOutermost = GetOuter() == GetOutermost();
	bool hasOwner = GetOwner() != nullptr;
	bool isBlueprintClass = Cast<UBlueprintGeneratedClass>(GetClass()) != nullptr;
	bool isClassConfig = !hasOwner && isBlueprintClass && isOutermost;

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneCastStateMachine, hasMultipleSlots) ||
		InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneCastStateMachine, fixedSlotCount))
	{
		return result && isClassConfig;
	}

	if (InProperty->GetFName() == GET_MEMBER_NAME_CHECKED(URuneCastStateMachine, slots))
	{
		return result && (isClassConfig || (!isClassConfig && !fixedSlotCount));
	}

	return result;
}
#endif

void URuneCastStateMachine::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);

	// TODO: state machine related policies:
	// - Allow tick when theres more than one pending state
	// - Flush all pending states in a single frame or one state per frame
	// - etc.
	// For now, we control a state per frame

	// check for pending states if NEXT_FRAME policy is set
	if (!_pendingStates.IsEmpty() && transitionPolicy == ETransitionPolicy::NEXT_TICK)
	{
		// change state and invoked all event delegates
		const UState* pendingState;
		_pendingStates.Dequeue(pendingState);

		// perform state change
		PerformTransition(pendingState);
	}

	// check if it is running (it can be paused in enter/exit)
	if (currentState != nullptr && IsRunning())
	{
		currentState->onTick.Broadcast(DeltaTime);
		onStateTick.Broadcast(currentState);
	}

	// check for pending states if END_FRAME policy is set
	if (!_pendingStates.IsEmpty() && transitionPolicy == ETransitionPolicy::END_TICK)
	{
		// change state and invoked all event delegates
		const UState* pendingState;
		_pendingStates.Dequeue(pendingState);

		// perform state change
		PerformTransition(pendingState);
	}
}

void URuneCastStateMachine::Init()
{
	// by default calls the blueprint version
	ReceiveInit();
}

void URuneCastStateMachine::SetEntryState(UState* state)
{
	//ASSERT(_entryState == nullptr, "Entry state can only be set once");
	if (_entryState != nullptr) return;

	//ASSERT(currentState == nullptr, "There should NOT be a running state at this point");
	if (currentState != nullptr) return;

	_entryState = state;

	// broadcast on enter
	if (state != nullptr)
	{
		state->onEnter.Broadcast();
		onStateEnter.Broadcast(state);
	}

	// change state
	currentState = state;
}

void URuneCastStateMachine::ChangeState(const UState* state)
{
	//ASSERT(currentState != nullptr, "Cannot change state without a state running or while transitioning");
	if (currentState == nullptr) return;

	// instant policy
	if (transitionPolicy == ETransitionPolicy::INSTANT)
	{
		PerformTransition(state);
		return;
	}

	// next tick policy
	if (transitionPolicy == ETransitionPolicy::NEXT_TICK)
	{
		// enqueue into the pending states
		_pendingStates.Enqueue(state);
		return;
	}

	// end tick policy
	if (transitionPolicy == ETransitionPolicy::END_TICK)
	{
		// enqueue into the pending states
		_pendingStates.Enqueue(state);
		return;
	}
}

void URuneCastStateMachine::SetPressed()
{
	isPressed = true;
	onPress.Broadcast();
}

void URuneCastStateMachine::SetReleased()
{
	isPressed = false;
	onRelease.Broadcast();
}

void URuneCastStateMachine::Resume()
{
	if (IsComponentTickEnabled()) return;

	SetComponentTickEnabled(true);
	onResume.Broadcast();
}

void URuneCastStateMachine::Pause()
{
	if (!IsComponentTickEnabled()) return;

	SetComponentTickEnabled(false);
	onPause.Broadcast();
}

bool URuneCastStateMachine::IsRunning() const
{
	return IsComponentTickEnabled();
}

void URuneCastStateMachine::SetLinkedBehaviour(TArray<URuneBehaviour*> newBehaviours)
{
	for (URuneBehaviour* behaviour : runeBehaviours)
	{
		if (!newBehaviours.Contains(behaviour))
			behaviour->DestroyComponent();
	}
	runeBehaviours = newBehaviours;
}

UState* URuneCastStateMachine::CreateState(FName name)
{
	UState* state = NewObject<UState>(this, UState::StaticClass());
	if (state == nullptr)
	{
		return nullptr;
	}
	state->nameID = name;
	_states.Add(state);

	return state;
}

void URuneCastStateMachine::DestroyState(UState* state)
{
	if (state == nullptr)
	{
		return;
	}

	if (!_states.Contains(state))
	{
		return;
	}

	// GC will handle the destroy state of the UObject
	_states.Remove(state);
}

UState* URuneCastStateMachine::GetState(FName name) const
{
	UState* const* foundState = _states.FindByPredicate([&name](UState* state) { return state != nullptr && state->nameID == name; });
	return foundState != nullptr ? *foundState : nullptr;
}

TArray<UState*> URuneCastStateMachine::GetStatesByName(FName name) const
{
	TArray<UState*> states;
	for (UState* state : _states)
	{
		if (state->nameID != name) continue;
		states.Add(state);
	}

	return states;
}

const TArray<UState*>& URuneCastStateMachine::GetStates() const
{
	return _states;
}

UState* URuneCastStateMachine::GetCurrentState() const
{
	return const_cast<UState*>(currentState);
}

bool URuneCastStateMachine::ActivateBehaviourSlot(int index)
{
	int count = GetBehaviourSlotCount();
	//ASSERT(!(index < 0 || index >= count), "Slot index is out of bounds");
	if (index < 0 || index >= count) return false;

	//ASSERT(runeBehaviours[index] != nullptr, "RuneBehaviour is null in slot %d", index);
	if (runeBehaviours[index] == nullptr) return false;

	runeBehaviours[index]->InternalActivateBehaviour();

	return true;
}

bool URuneCastStateMachine::DeactivateBehaviourSlot(int index)
{
	int count = GetBehaviourSlotCount();
	//ASSERT(!(index < 0 || index >= count), "Slot index is out of bounds");
	if (index < 0 || index >= count) return false;

	//ASSERT(runeBehaviours[index] != nullptr, "RuneBehaviour is null in slot %d", index);
	if (runeBehaviours[index] == nullptr) return false;

	runeBehaviours[index]->InternalDeactivateBehaviour();

	return true;
}

bool URuneCastStateMachine::ResetBehaviourSlot(int index)
{
	int count = GetBehaviourSlotCount();
	//ASSERT(!(index < 0 || index >= count), "Slot index is out of bounds");
	if (index < 0 || index >= count) return false;

	//ASSERT(runeBehaviours[index] != nullptr, "RuneBehaviour is null in slot %d", index);
	if (runeBehaviours[index] == nullptr) return false;

	runeBehaviours[index]->InternalResetBehaviour();

	return true;
}

bool URuneCastStateMachine::ShowBehaviourSlotPreview(int index)
{
	int count = GetBehaviourSlotCount();
	//ASSERT(!(index < 0 || index >= count), "Slot index is out of bounds");
	if (index < 0 || index >= count) return false;

	//ASSERT(runeBehaviours[index] != nullptr, "RuneBehaviour is null in slot %d", index);
	if (runeBehaviours[index] == nullptr) return false;

	return runeBehaviours[index]->InternalShowPreview();
}

bool URuneCastStateMachine::HideBehaviourSlotPreview(int index)
{
	int count = GetBehaviourSlotCount();
	//ASSERT(!(index < 0 || index >= count), "Slot index is out of bounds");
	if (index < 0 || index >= count) return false;

	//ASSERT(runeBehaviours[index] != nullptr, "RuneBehaviour is null in slot %d", index);
	if (runeBehaviours[index] == nullptr) return false;

	return runeBehaviours[index]->InternalHidePreview();
}

bool URuneCastStateMachine::IsBehaviourSlotPreviewShowing(int index)
{
	int count = GetBehaviourSlotCount();
	//ASSERT(!(index < 0 || index >= count), "Slot index is out of bounds");
	if (index < 0 || index >= count) return false;
	if (runeBehaviours.Num() != count) return false;

	//ASSERT(runeBehaviours[index] != nullptr, "RuneBehaviour is null in slot %d", index);
	if (runeBehaviours[index] == nullptr) return false;

	return runeBehaviours[index]->IsPreviewShowing();
}

bool URuneCastStateMachine::IsBehaviourSlotValid(int index) const
{
	int count = GetBehaviourSlotCount();
	if (index < 0 || index >= count) return false;

	return runeBehaviours[index] != nullptr;
}

const TArray<URuneBehaviour*>& URuneCastStateMachine::GetBehaviourSlots() const
{
	return runeBehaviours;
}

URuneBehaviour* URuneCastStateMachine::GetBehaviourSlot(int index) const
{
	int count = GetBehaviourSlotCount();
	//ASSERT(!(index < 0 || index >= count), "Slot index is out of bounds");
	if (index < 0 || index >= count) return nullptr;

	return runeBehaviours[index];
}

int32 URuneCastStateMachine::GetBehaviourSlotCount() const
{
	return hasMultipleSlots ? slots : 1;
}

void URuneCastStateMachine::PerformTransition(const UState* state)
{
	//ASSERT(state != nullptr, "Cannot transition into a null State");
	if (state == nullptr) return;

	const UState* prevState = currentState;
	currentState = nullptr;

	// broadcast on exit
	if (prevState != nullptr)
	{
		prevState->onExit.Broadcast();
		onStateExit.Broadcast(prevState);
	}

	// broadcast on enter
	if (state != nullptr)
	{
		state->onEnter.Broadcast();
		onStateEnter.Broadcast(state);
	}

	// change state
	currentState = state;
}
