// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Misc/Guid.h"
#include "RuneEffectPayload.h"
#include "RuneEffectHandle.generated.h"


class URuneEffect;

/**
 * 
 */
USTRUCT(BlueprintType)
struct FRuneEffectHandle
{
	GENERATED_BODY()

public:
	FRuneEffectHandle() = default;
	FRuneEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload);

public:
	UPROPERTY()
	FGuid ID;

	UPROPERTY()
	uint64 TimeStamp;

	UPROPERTY()
	URuneEffect* Effect;

	UPROPERTY()
	FRuneEffectPayload Payload;

public:
	static const FRuneEffectHandle Invalid;

};
