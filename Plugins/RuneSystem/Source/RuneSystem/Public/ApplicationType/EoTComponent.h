

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "EoTComponent.generated.h"


class URuneEffect;

UCLASS(ClassGroup = (Custom), Blueprintable, meta = (BlueprintSpawnableComponent, DisplayName = "EoT Component"))
class RUNESYSTEM_API UEoTComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UEoTComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	//void Configure(const TSubclassOf<URuneEffect>& effectClass, AActor* instigator, uint32 ticks = 5, float duration = 5.0f, bool trimTickDistribution = true, float tickRate = 0.5f);
	void Configure(URuneEffect* effect, AController* instigator, uint32 ticks = 5, float duration = 5.0f, bool trimTickDistribution = true, float tickRate = 0.5f);
	UFUNCTION()
	void RemoveFromOwner(AActor* owner);

private:
	void ApplyTickEffect();

public:
	UPROPERTY(EditAnywhere, Category = "EoTComponent: General Settings", meta = (EditCondition = "runeEffect==nullptr", EditConditionHides))
	TSubclassOf<URuneEffect> runeEffectClass;

	UPROPERTY(EditAnywhere, Category = "EoTComponent: General Settings", Instanced, meta = (EditCondition = "runeEffectClass==nullptr", EditConditionHides))
	URuneEffect* runeEffect;

	UPROPERTY(EditAnywhere, Category = "EoTComponent: General Settings")
	uint32 ticks;

	UPROPERTY(EditAnywhere, Category = "EoTComponent: General Settings")
	float duration;

	UPROPERTY(EditAnywhere, Category = "EoTComponent: General Settings")
	bool trimTickDistribution;

	UPROPERTY(EditAnywhere, Category = "EoTComponent: General Settings")
	float tickRate;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "EoTComponent: Debug Variables")
	AController* _instigator;

	UPROPERTY(VisibleInstanceOnly, Category = "EoTComponent: Debug Variables")
	float _timePerTick;

	UPROPERTY(VisibleInstanceOnly, Category = "EoTComponent: Debug Variables")
	int32 _remainingTicks;

	UPROPERTY()
	FTimerHandle _timeHandle;
};
