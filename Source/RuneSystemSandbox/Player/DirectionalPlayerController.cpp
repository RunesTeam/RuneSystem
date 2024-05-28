
#include "Player/DirectionalPlayerController.h"
#include "Input/InputBindingsDeveloperSettings.h"
#include "EnhancedInputComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

const FRuneInput& ADirectionalPlayerController::GetRuneInput()
{
	if(playerCharacter != nullptr)
	{
		runeInput.aimDirection = playerCharacter->GetActorForwardVector();
		runeInput.inputDirection = playerCharacter->GetActorForwardVector();
		runeInput.originLocation = playerCharacter->GetActorLocation();
	}
	return runeInput;
}

const URuneFilter* ADirectionalPlayerController::GetRuneFilter() const
{
	return runeFilter;
}

AController* ADirectionalPlayerController::GetController() const
{
	return const_cast<AController*>(Cast<AController>(this));
}

void ADirectionalPlayerController::SetupInputComponent()
{
	Super::SetupInputComponent();

	// Make sure that we are using a UEnhancedInputComponent; if not, the project is not configured correctly.
	if (UEnhancedInputComponent* EnhancedInputCmp = Cast<UEnhancedInputComponent>(InputComponent))
	{
		const UInputBindingsDeveloperSettings* inputSettings = GetDefault<UInputBindingsDeveloperSettings>();
		if (inputSettings == nullptr)
		{
			return;
		}
		// Movement and dash
		EnhancedInputCmp->BindAction(inputSettings->GetInputActionByName("HorizontalAxis"), ETriggerEvent::Triggered, this, &ADirectionalPlayerController::HorizontalAxis);
		EnhancedInputCmp->BindAction(inputSettings->GetInputActionByName("HorizontalAxis"), ETriggerEvent::Completed, this, &ADirectionalPlayerController::HorizontalAxis);
		EnhancedInputCmp->BindAction(inputSettings->GetInputActionByName("VerticalAxis"), ETriggerEvent::Triggered, this, &ADirectionalPlayerController::VerticalAxis);
		EnhancedInputCmp->BindAction(inputSettings->GetInputActionByName("VerticalAxis"), ETriggerEvent::Completed, this, &ADirectionalPlayerController::VerticalAxis);
#if WITH_EDITOR
		EnhancedInputCmp->BindAction(inputSettings->GetInputActionByName("PauseEditor"), ETriggerEvent::Started, this, &ADirectionalPlayerController::Pause);
#else
		EnhancedInputCmp->BindAction(inputSettings->GetInputActionByName("Pause"), ETriggerEvent::Started, this, &AVibingPlayerController::Pause);
#endif
	}
}

void ADirectionalPlayerController::HorizontalAxis(const FInputActionValue& Value)
{
	xMoveAxisValue = Value.Get<FVector2D>().X;
}

void ADirectionalPlayerController::VerticalAxis(const FInputActionValue& Value)
{
	yMoveAxisValue = Value.Get<FVector2D>().X;
}

void ADirectionalPlayerController::Tick(float DeltaTime)
{
	GetInputVectors();
	if (!UGameplayStatics::IsGamePaused(GetWorld()) && playerCharacter)
	{
		playerCharacter->AddMovementInput(inputMove);
		//playerCharacter->ApplyRotation(desiredDirection);
	}
}

void ADirectionalPlayerController::GetInputVectors()
{
	inputMove = (FVector::RightVector * xMoveAxisValue) + (FVector::ForwardVector * yMoveAxisValue);

	//inputMove = inputMove.RotateAngleAxis(-45, FVector::UpVector);

	inputMove.Normalize();
}

void ADirectionalPlayerController::OnPossess(APawn* aPawn)
{
	Super::OnPossess(aPawn);

	playerCharacter = Cast<ACharacter>(aPawn);
}

void ADirectionalPlayerController::Pause()
{
	onPause.Broadcast();
}
