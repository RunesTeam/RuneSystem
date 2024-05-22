// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Templates/Invoke.h"
#include "Effect/RuneEffectHandle.h"
#include "RuneSystem.generated.h"


class URuneEffect;
struct FRuneEffectPayload;


UCLASS()
class RUNESYSTEM_API URuneSystem : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	// Default values
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
	/**
	 * Creates an instance of an effect.
	 *
	 * @param EffectClass Effect to instantiate
	 * @param OutApplicationModeInstance Application Mode instance
	 * @param OverrideApplicationModeClass Application Mode class to override with. If None, there is no override.
	 * @return Rune Effect instance
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect", meta = (HideSelfPin, WorldContext = "WorldContextObject"))
	static URuneEffect* CreateEffectInstance(
		UObject* WorldContextObject,
		UPARAM(meta = (AllowAbstract = "false")) const TSubclassOf<class URuneEffect> EffectClass,
		UPARAM(DisplayName = "Application Mode") class URuneEffectApplicationMode*& OutApplicationModeInstance,
		const TSubclassOf<class URuneEffectApplicationMode> OverrideApplicationModeClass = nullptr);

	/**
	 * Activates an effect with a given payload.
	 *
	 * @param EffectClass Effect to be applied
	 * @param Payload Application data payload
	 * @param OutEffect Applied effect instance.
	 * @return Handle representing the application of the effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect", meta = (WorldContext = "WorldContextObject", DeterminesOutputType = "EffectClass", DynamicOutputParam = "OutEffect", HideSelfPin))
	static FRuneEffectHandle ActivateEffectByClass(
		UObject* WorldContextObject,
		UPARAM(meta = (AllowAbstract = "false")) const TSubclassOf<class URuneEffect> EffectClass,
		const FRuneEffectPayload& Payload,
		class URuneEffect*& OutEffect);

	/**
	 * Activates an effect with a given payload.
	 *
	 * @param Effect Effect instance to be applied
	 * @param Payload Application data payload
	 * @return Handle representing the application of the effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect", meta = (DefaultToSelf = "Effect", HideSelfPin))
	static FRuneEffectHandle ActivateEffect(class URuneEffect* Effect, const FRuneEffectPayload& Payload);

	/**
	 * Deactivates an effect given its application handle.
	 *
	 * @param Handle Effect application to be deactivated
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect", meta = (DefaultToSelf = "Effect", HideSelfPin))
	static void DeactivateEffect(const FRuneEffectHandle& Handle);

	/**
	 * Creates a new activation handle given an effect and its payload.
	 * Manual dispose is required.
	 * 
	 * @param Effect Effect the activation handle represents
	 * @param Payload Effect payload
	 * @return An effect application handle
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect", meta = (CallableWithoutWorldContext))
	static FRuneEffectHandle& CreateEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload);

	/**
	 * Dispose an effect application handle.
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect", meta = (CallableWithoutWorldContext))
	static void DisposeEffectHandle(const FRuneEffectHandle& Handle);

	/**
	 * Gets all effect handles that matches the given predicate.
	 *
	 * @param WorldContextObject World Context
	 * @return All matching handles
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect", meta = (WorldContext = "WorldContextObject"))
	static const TArray<FRuneEffectHandle>& GetEffectHandles(const UObject* WorldContextObject);

	/**
	 * Gets the application data object associated to a given handle.
	 * 
	 * @param WorldContextObject World Context
	 * @param Handle A valid effect application handle
	 * @return Object associated to the given handle
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect Application Mode", meta = (WorldContext = "WorldContextObject"))
	static UObject* GetEffectHandleApplicationData(const UObject* WorldContextObject, const FRuneEffectHandle& Handle);

	/**
	 * Sets an application data object associated to a given handle.
	 *
	 * @param WorldContextObject World Context
	 * @param Handle A valid effect application handle
	 * @param Data A valid effect application handle
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Effect Application Mode", meta = (WorldContext = "WorldContextObject"))
	static void SetEffectHandleApplicationData(const UObject* WorldContextObject, const FRuneEffectHandle& Handle, UObject* Data);

	/**
	 * Gets all effect handles that matches the given predicate.
	 * 
	 * @return All matching handles
	 */
	template<typename Predicate>
	static TArray<FRuneEffectHandle> GetEffectHandlesByPredicate(const UObject* WorldContextObject, Predicate Pred);

private:
	static URuneSystem* GetRuneSystem(const UObject* WorldContextObject);

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

};

template<typename Predicate>
inline TArray<FRuneEffectHandle> URuneSystem::GetEffectHandlesByPredicate(const UObject* WorldContextObject, Predicate Pred)
{
	TArray<FRuneEffectHandle> Result;
	for (const FRuneEffectHandle& Handle : GetRuneSystem(WorldContextObject)->ActiveHandles)
	{
		if (::Invoke(Pred, Handle))
		{
			Result.Add(Handle);
		}
	}

	return Result;
}
