


#include "RuneSystem.h"

#include "RuneEffect.h"


URuneSystem* URuneSystem::s_Instance = nullptr;

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

	check(s_Instance == nullptr);
	s_Instance = this;
}

void URuneSystem::Deinitialize()
{
	Super::Deinitialize();

	s_Instance = nullptr;
}

bool URuneSystem::DoesSupportWorldType(EWorldType::Type WorldType) const
{
	return WorldType == EWorldType::Game || WorldType == EWorldType::PIE;
}

FRuneEffectHandle& URuneSystem::CreateEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload)
{
	return s_Instance->CreateEffectHandleImpl(Effect, Payload);
}

void URuneSystem::DisposeEffectHandle(const FRuneEffectHandle& Handle)
{
	s_Instance->DisposeEffectHandleImpl(Handle);
}

UObject* URuneSystem::GetEffectHandleApplicationData(const FRuneEffectHandle& Handle)
{
	return s_Instance->GetEffectHandleApplicationDataImpl(Handle);
}

void URuneSystem::SetEffectHandleApplicationData(const FRuneEffectHandle& Handle, UObject* Data)
{
	check(Handle.ID.IsValid() && IsValid(Handle.Effect));
	if (!IsValid(Data)) return;
	s_Instance->SetEffectHandleApplicationDataImpl(Handle, Data);
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
