// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "InputActionValue.h"
#include "RuneCompatible.h"
#include "DirectionalPlayerController.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FPause);

UCLASS()
class RUNESYSTEMSANDBOX_API ADirectionalPlayerController : public APlayerController, public IRuneCompatible
{
	GENERATED_BODY()
public:
	// Inherited via IRuneCompatible
	UFUNCTION(BlueprintCallable)
	virtual const FRuneInput& GetRuneInput() override;

	UFUNCTION(BlueprintCallable)
	virtual const URuneFilter* GetRuneFilter() const override;

	UFUNCTION(BlueprintCallable)
	virtual AController* GetController() const override;

protected:
	// PlayerController
	void SetupInputComponent() override;
	virtual void Tick(float DeltaTime) override;
	virtual void OnPossess(APawn* aPawn) override;
	void GetInputVectors();

	// ADirectionalPlayerController
	void HorizontalAxis(const FInputActionValue& Value);
	void VerticalAxis(const FInputActionValue& Value);
	void Pause();

	float xMoveAxisValue;
	float yMoveAxisValue;
	ACharacter* playerCharacter;
	FVector inputMove;
public:
	UPROPERTY(BlueprintCallable, BlueprintAssignable)
	FPause onPause;
protected:
	FRuneInput runeInput;
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	URuneFilter* runeFilter;
};
