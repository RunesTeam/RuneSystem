


#include "RunePreviewAgent.h"


ARunePreviewAgent::ARunePreviewAgent() :
	isInitializedHidden(true)
{
	PrimaryActorTick.bCanEverTick = true;
}

void ARunePreviewAgent::BeginPlay()
{
	Super::BeginPlay();
}

void ARunePreviewAgent::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ARunePreviewAgent::Show()
{
	SetActorHiddenInGame(false);
}

void ARunePreviewAgent::Hide()
{
	SetActorHiddenInGame(true);
}

bool ARunePreviewAgent::IsVisible() const
{
	return !IsHidden();
}

bool ARunePreviewAgent::IsHidden() const
{
	return Super::IsHidden();
}

