// Fill out your copyright notice in the Description page of Project Settings.


#include "RuneEffectHandle.h"


const FRuneEffectHandle FRuneEffectHandle::Invalid = {};

FRuneEffectHandle::FRuneEffectHandle(URuneEffect* Effect, const FRuneEffectPayload& Payload) :
	ID(FGuid::NewGuid()),
	Effect(Effect),
	Payload(Payload)
{}