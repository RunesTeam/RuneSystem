// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include <Templates/Invoke.h>
#include "RuneEffectHandle.h"
#include "RuneSystem.generated.h"


class URuneEffect;
struct FRuneEffectPayload;


UCLASS()
class RUNESYSTEM_API URuneSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	URuneSystem();

	/** Override to control if the Subsystem should be created at all.
	 * For example you could only have your system created on servers.
	 * It's important to note that if using this is becomes very important to null check whenever getting the Subsystem.
	 *
	 * Note: This function is called on the CDO prior to instances being created!
	 *
	 */
	virtual bool ShouldCreateSubsystem(UObject* Outer) const override;

	/** Implement this for initialization of instances of the system */
	virtual void Initialize(FSubsystemCollectionBase& Collection) override;

	/** Implement this for deinitialization of instances of the system */
	virtual void Deinitialize() override;

protected:
	/** Defines which world type is valid for this UWorldSubsystem */
	virtual bool DoesSupportWorldType(EWorldType::Type WorldType) const override;

public:
	static FRuneEffectHandle& CreateEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload);
	static void DisposeEffectHandle(const FRuneEffectHandle& Handle);

	template<typename Predicate>
	static TArray<FRuneEffectHandle> GetEffectHandlesByPredicate(Predicate Pred);

	static UObject* GetEffectHandleApplicationData(const FRuneEffectHandle& Handle);
	static void SetEffectHandleApplicationData(const FRuneEffectHandle& Handle, UObject* Data);

private:
	FRuneEffectHandle& CreateEffectHandleImpl(URuneEffect* Effect, const FRuneEffectPayload& Payload);
	void DisposeEffectHandleImpl(const FRuneEffectHandle& Handle);

	UObject* GetEffectHandleApplicationDataImpl(const FRuneEffectHandle& Handle);
	void SetEffectHandleApplicationDataImpl(const FRuneEffectHandle& Handle, UObject* Data);

private:
	UPROPERTY()
	TArray<FRuneEffectHandle> ActiveHandles;

	UPROPERTY()
	TMap<FGuid, UObject*> HandleApplicationData;

private:
	static URuneSystem* s_Instance;

};

template<typename Predicate>
inline TArray<FRuneEffectHandle> URuneSystem::GetEffectHandlesByPredicate(Predicate Pred)
{
	TArray<FRuneEffectHandle> Result;
	for (const FRuneEffectHandle& Handle : s_Instance->ActiveHandles)
	{
		if (::Invoke(Pred, Handle))
		{
			Result.Add(Handle);
		}
	}

	return Result;
}
