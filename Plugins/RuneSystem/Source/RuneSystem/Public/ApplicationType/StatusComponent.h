

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "StatusComponent.generated.h"


class URuneEffect;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class RUNESYSTEM_API UStatusComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UStatusComponent();

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:
	//void Configure(const TSubclassOf<URuneEffect>& effectClass, float duration = 5.0f);
	void Configure(URuneEffect* effect, AController* _instigator, float duration = 5.0f);

private:
	void BeginStatusEffect();
	void EndStatusEffect();

public:
	UPROPERTY(EditAnywhere, Category = "StatusComponent: General Settings", meta = (EditCondition = "runeEffect==nullptr", EditConditionHides))
	TSubclassOf<URuneEffect> runeEffectClass;

	UPROPERTY(EditAnywhere, Category = "StatusComponent: General Settings", meta = (EditCondition = "runeEffectClass==nullptr", EditConditionHides))
	URuneEffect* runeEffect;

	UPROPERTY(EditAnywhere, Category = "StatusComponent: General Settings")
	float duration;

private:
	UPROPERTY(VisibleInstanceOnly, Category = "EoTComponent: Debug Variables")
	AController* _instigator;
	UPROPERTY()
	FTimerHandle _timeHandle;
		
};
