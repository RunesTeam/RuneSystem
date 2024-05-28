
#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "RuneTask.generated.h"

// TODO: This enum is NOT a good solution
UENUM()
enum class ERuneTaskReturnValue
{
	SUCCESS,
	FAILURE,
	RUNNING,
	HOLD,
};

class URuneBaseComponent;

UCLASS(Abstract, Blueprintable, EditInlineNew, HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking, "Components|Activation"))
class RUNESYSTEM_API URuneTask : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	URuneTask();

	UFUNCTION(BlueprintCallable)
	virtual ERuneTaskReturnValue Evaluate() const;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UFUNCTION(BlueprintCallable)
	virtual void Configure(const URuneBaseComponent* runeBaseComponent);

	UFUNCTION(BlueprintImplementableEvent, meta = (DisplayName = "Configure"))
	void ReceiveConfigure(const URuneBaseComponent* runeBaseComponent);

protected:
	UPROPERTY(BlueprintReadWrite)
	TEnumAsByte <ERuneTaskReturnValue> currentReturnValue;
		
};
