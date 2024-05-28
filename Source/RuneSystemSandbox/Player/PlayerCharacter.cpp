// Fill out your copyright notice in the Description page of Project Settings.


#include "Player/PlayerCharacter.h"
#include "EnhancedInputSubsystems.h"
#include "InputMappingContext.h"
#include "RuneBaseComponent.h"

// Sets default values
APlayerCharacter::APlayerCharacter()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
	for (UActorComponent* actorComp : GetComponents())
	{
		if (actorComp != nullptr && actorComp->IsA<URuneBaseComponent>())
		{
			Cast<URuneBaseComponent>(actorComp)->SetOwner(Cast<IRuneCompatible>(Controller));
		}
	}
}

// Called every frame
void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

// Called to bind functionality to input
void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
	// Get the player controller
	if (APlayerController* PC = GetController<APlayerController>())
	{
		const ULocalPlayer* localPlayer = PC->GetLocalPlayer();
		// Get the Enhanced Input Local Player Subsystem from the Local Player related to our Player Controller.
		UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(localPlayer);
		if (Subsystem != nullptr)
		{
			// PawnClientRestart can run more than once in an Actor's lifetime, so start by clearing out any leftover mappings.
			Subsystem->ClearAllMappings();
			// Add each mapping context, along with their priority values. Higher values outprioritize lower values.0
			Subsystem->AddMappingContext(mappingContext, 0);
			//Subsystem->AddMappingContext(defaultMappingContext, 0);
		}
	}
}