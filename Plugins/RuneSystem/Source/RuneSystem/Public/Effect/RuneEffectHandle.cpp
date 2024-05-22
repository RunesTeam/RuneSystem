// Fill out your copyright notice in the Description page of Project Settings.


#include "RuneEffectHandle.h"
#include "Misc/DateTime.h"


const FRuneEffectHandle FRuneEffectHandle::Invalid = {};

FRuneEffectHandle::FRuneEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload) :
	ID(FGuid::NewGuid()),
	TimeStamp(FDateTime::UtcNow().ToUnixTimestamp()),
	Effect(Effect),
	Payload(Payload)
{
}
