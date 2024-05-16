// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "RuneFilter.h"
#include "RuneEffectPayload.generated.h"


/**
 * 
 */
USTRUCT(BlueprintType, meta = (HasNativeMake = "RuneSystem.RuneEffectPayloadHelperLibrary.MakeRuneEffectPayload"))
struct FRuneEffectPayload
{
	GENERATED_BODY()

	FRuneEffectPayload() :
		Target(nullptr),
		RuneFilter(nullptr),
		Instigator(nullptr),
		Causer(nullptr),
		UserData(nullptr)
	{}

	UPROPERTY(BlueprintReadOnly)
	AActor* Target;

	UPROPERTY(BlueprintReadOnly)
	URuneFilter* RuneFilter;

	UPROPERTY(BlueprintReadOnly)
	AActor* Instigator;

	UPROPERTY(BlueprintReadOnly)
	AActor* Causer;

	UPROPERTY(BlueprintReadOnly)
	UObject* UserData;

};

UCLASS()
class URuneEffectPayloadHelperLibrary : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:
	UFUNCTION(BlueprintPure, Category = "Rune System|Rune Effect", meta = (BlueprintThreadSafe))
	static FRuneEffectPayload MakeRuneEffectPayload(
		AActor* Target,
		URuneFilter* RuneFilter,
		AActor* Instigator,
		AActor* Causer,
		UObject* UserData
	);

};

inline FRuneEffectPayload URuneEffectPayloadHelperLibrary::MakeRuneEffectPayload(AActor* Target, URuneFilter* RuneFilter, AActor* Instigator, AActor* Causer, UObject* UserData)
{
	FRuneEffectPayload Payload;
	Payload.Target = Target;
	Payload.RuneFilter = RuneFilter;
	Payload.Instigator = Instigator;
	Payload.Causer = Causer;
	Payload.UserData = UserData;

	return Payload;
}
