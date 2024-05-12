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

	FRuneEffectHandle() = default;
	FRuneEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload) :
		ID(FGuid::NewGuid()),
		Effect(Effect),
		Payload(Payload)
	{}

public:
	UPROPERTY()
	FGuid ID;

	UPROPERTY()
	URuneEffect* Effect;

	UPROPERTY()
	FRuneEffectPayload Payload;

public:
	static const FRuneEffectHandle Invalid;

};

inline const FRuneEffectHandle FRuneEffectHandle::Invalid = {};
