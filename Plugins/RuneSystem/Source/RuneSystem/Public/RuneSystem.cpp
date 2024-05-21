


#include "RuneSystem.h"

#include "Effect/RuneEffect.h"
#include "Effect/ApplicationMode/RuneEffectApplicationMode.h"


URuneSystem::URuneSystem() :
	ActiveHandles(),
	HandleApplicationData()
{
}

bool URuneSystem::ShouldCreateSubsystem(UObject* Outer) const
{
	return Super::ShouldCreateSubsystem(Outer);
}

void URuneSystem::Initialize(FSubsystemCollectionBase& Collection)
{
	Super::Initialize(Collection);
}

void URuneSystem::Deinitialize()
{
	Super::Deinitialize();
}

bool URuneSystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

URuneEffect* URuneSystem::CreateEffectInstance(UObject* WorldContextObject, const TSubclassOf<class URuneEffect> EffectClass, class URuneEffectApplicationMode*& OutApplicationModeInstance, const TSubclassOf<class URuneEffectApplicationMode> OverrideApplicationModeClass)
{
	if (!IsValid(EffectClass))
	{
		return nullptr;
	}

	URuneEffect* OutEffect = NewObject<URuneEffect>(WorldContextObject, EffectClass, TEXT("Rune Effect"), RF_Transient);
	if (IsValid(OutEffect) && IsValid(OverrideApplicationModeClass))
	{
		OutEffect->ApplicationMode = NewObject<URuneEffectApplicationMode>(OutEffect, OverrideApplicationModeClass, TEXT("Rune Effect Application Mode"), RF_Transient);
		check(IsValid(OutEffect->ApplicationMode));
	}

	return OutEffect;
}

FRuneEffectHandle URuneSystem::ActivateEffectByClass(UObject* WorldContextObject, const TSubclassOf<URuneEffect> EffectClass, const FRuneEffectPayload& Payload, URuneEffect*& OutEffect)
{
	if (!IsValid(EffectClass) || !IsValid(Payload.Target))
	{
		return FRuneEffectHandle::Invalid;
	}

	OutEffect = NewObject<URuneEffect>(WorldContextObject, EffectClass, TEXT("Rune Effect"), RF_Transient);
	check(IsValid(OutEffect));

	return OutEffect->Activate(Payload);
}

FRuneEffectHandle URuneSystem::ActivateEffect(URuneEffect* Effect, const FRuneEffectPayload& Payload)
{
	if (!IsValid(Effect) || !IsValid(Payload.Target))
	{
		return FRuneEffectHandle::Invalid;
	}

	return Effect->Activate(Payload);
}

void URuneSystem::DeactivateEffect(const FRuneEffectHandle& Handle)
{
	if (!IsValid(Handle.Effect) || !IsValid(Handle.Payload.Target))
	{
		return;
	}

	Handle.Effect->Deactivate(Handle);
}


FRuneEffectHandle& URuneSystem::CreateEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload)
{
	return GetRuneSystem(Effect)->CreateEffectHandleImpl(Effect, Payload);
}

void URuneSystem::DisposeEffectHandle(const FRuneEffectHandle& Handle)
{
	GetRuneSystem(Handle.Effect)->DisposeEffectHandleImpl(Handle);
}

const TArray<FRuneEffectHandle>& URuneSystem::GetEffectHandles(const UObject* WorldContextObject)
{
	return GetRuneSystem(WorldContextObject)->ActiveHandles;
}

UObject* URuneSystem::GetEffectHandleApplicationData(const UObject* WorldContextObject, const FRuneEffectHandle& Handle)
{
	return GetRuneSystem(WorldContextObject)->GetEffectHandleApplicationDataImpl(Handle);
}

void URuneSystem::SetEffectHandleApplicationData(const UObject* WorldContextObject, const FRuneEffectHandle& Handle, UObject* Data)
{
	check(Handle.ID.IsValid() && IsValid(Handle.Effect));
	if (!IsValid(Data)) return;
	GetRuneSystem(WorldContextObject)->SetEffectHandleApplicationDataImpl(Handle, Data);
}

URuneSystem* URuneSystem::GetRuneSystem(const UObject* WorldContextObject)
{
	check(IsValid(WorldContextObject));

	UWorld* World = WorldContextObject->GetWorld();
	check(IsValid(World));
	check(World->HasSubsystem<URuneSystem>());

	return World->GetSubsystem<URuneSystem>();
}

FRuneEffectHandle& URuneSystem::CreateEffectHandleImpl(URuneEffect* Effect, const FRuneEffectPayload& Payload)
{
	int32 Index = ActiveHandles.Emplace(Effect, Payload);
	return ActiveHandles[Index];
}

void URuneSystem::DisposeEffectHandleImpl(const FRuneEffectHandle& Handle)
{
	ActiveHandles.RemoveAll([&Handle](const FRuneEffectHandle& ElemHandle)
		{
			return ElemHandle.ID == Handle.ID;
		});

	HandleApplicationData.Remove(Handle.ID);
}

UObject* URuneSystem::GetEffectHandleApplicationDataImpl(const FRuneEffectHandle& Handle)
{
	if (!HandleApplicationData.Contains(Handle.ID))
	{
		return nullptr;
	}

	return *HandleApplicationData.Find(Handle.ID);
}

void URuneSystem::SetEffectHandleApplicationDataImpl(const FRuneEffectHandle& Handle, UObject* Data)
{
	HandleApplicationData.Add(Handle.ID, Data);
}
