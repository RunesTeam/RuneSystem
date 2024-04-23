

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "RuneFilter.h"
#include "RuneCompatible.generated.h"

USTRUCT(Blueprintable, BlueprintType)
struct FRuneInput
{
	GENERATED_BODY()
	
	/** Forward vector of rune instigator. Normalized. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector aimDirection;

	/** Direction towards rune should be casted. Not normalized. Can be zero. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector inputDirection;

	/** Location where the cast should be casted */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	FVector originLocation;

	/** Whether the AimDirection magnitude maxes out at one or not */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	bool isMaxMagnitudeNormalized;

	/** Target locked by controller aim, allows for behaviour-specific aim assistance */
	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	AActor* lockedTarget;
};

// This class does not need to be modified.
UINTERFACE(MinimalAPI, meta = (CannotImplementInterfaceInBlueprint))
class URuneCompatible : public UInterface
{
	GENERATED_BODY()
};


class RUNESYSTEM_API IRuneCompatible
{
	GENERATED_BODY()

	// Add interface functions to this class. This is the class that will be inherited to implement this interface.
public:
	/**
	 * Gets a reference to the data related to the input.
	 * TODO: it should be a const method (a standard getter)
	 * 
	 * @return Input struct.
	 */
	UFUNCTION(BlueprintCallable)
	virtual const FRuneInput& GetRuneInput() = 0;

	/**
	 * Gets a pointer to the filter of the instigator. It could be nullptr.
	 * 
	 * @return RuneFilter
	 */
	UFUNCTION(BlueprintCallable)
	virtual const URuneFilter* GetRuneFilter() const = 0;

	/**
	 * Gets a pointer to the rune compatible controller. It could be nullptr.
	 *
	 * @return RuneFilter
	 */
	UFUNCTION(BlueprintCallable)
	virtual AController* GetController() const = 0;
};
