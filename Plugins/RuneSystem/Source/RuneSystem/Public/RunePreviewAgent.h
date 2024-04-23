

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RunePreviewAgent.generated.h"

UCLASS(Abstract, Blueprintable, HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking, "Components|Activation", Tick, Replication, Rendering, Actor, Input, LOD))
class RUNESYSTEM_API ARunePreviewAgent : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ARunePreviewAgent();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

public:
	UFUNCTION(BlueprintCallable)
	virtual void Show();

	UFUNCTION(BlueprintCallable)
	virtual void Hide();

	UFUNCTION(BlueprintCallable)
	virtual bool IsVisible() const;

	UFUNCTION(BlueprintCallable)
	virtual bool IsHidden() const;

protected:
	UPROPERTY(EditAnywhere)
	bool isInitializedHidden;

private:
	friend class URuneUtils;

};
