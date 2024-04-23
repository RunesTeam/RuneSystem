

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "RuneTangibleAgent.generated.h"


DECLARE_DYNAMIC_MULTICAST_DELEGATE_TwoParams(FAgentApplicationDelegate, AActor*, target, bool, success);


UCLASS(Abstract, Blueprintable, HideCategories = ("ComponentTick", Tags, AssetUserData, ComponentReplication, Activation, Variable, Sockets, Collision, Cooking, "Components|Activation", Tick, Replication, Rendering, Actor, Input, LOD))
class RUNESYSTEM_API ARuneTangibleAgent : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	ARuneTangibleAgent();

#if WITH_EDITOR
	/**
	 * Called by the editor to query whether a property of this object is allowed to be modified.
	 * The property editor uses this to disable controls for properties that should not be changed.
	 * When overriding this function you should always call the parent implementation first.
	 *
	 * @param InProperty The property to query
	 *
	 * @return true if the property can be modified in the editor, otherwise false
	 */
	virtual bool CanEditChange(const FProperty* InProperty) const override;
#endif

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	// Called every frame
	virtual void Tick(float DeltaTime) override;

protected:
	/**
	 * Iterates trhough all attached effects and tries to apply each one to
	 * the given actor. Whether the effect is applied or not is determined by
	 * their particular filter.
	 *
	 * @param actor Actor to which the effects should be applied
	 * @return true if any of the effects were succesfully applied
	 */
	UFUNCTION(BlueprintCallable)
	bool TryApplyEffects(AActor* actor);

	/**
	 * Iterates trhough all attached effects and tries to rever each one to
	 * the given actor. Whether the effect is reverted or not is determined by
	 * their particular filter.
	 *
	 * @param actor Actor to which the effects should be reverted
	 * @return true if any of the effects were succesfully reverted
	 */
	UFUNCTION(BlueprintCallable)
	bool TryRevertEffects(AActor* actor);

	/**
	 * Checks if any of the attached effects could be successfully applied to
	 * the given actor without doing so. Useful for implementing multiple-stepped
	 * agents like splash projectiles.
	 *
	 * @param actor Actor to which the effects could be applied
	 * @return If any of the effects could be successfully applied
	 */
	UFUNCTION(BlueprintCallable)
	bool CheckForApplicableEffects(AActor* actor);

	/**
	 * Creates a copy of the given rune effects.
	 *
	 * @param runeEffects Rune effects that should be copied.
	 */
	UFUNCTION(BlueprintCallable)
	void SetAttachedRuneEffects(TArray<class UObject*> runeEffects);

	/**
	 * Gets the assigned preview agent class.
	 * Could be nullptr if not set or invalid.
	 *
	 * @return Pointer to the preview agent class.
	 */
	UFUNCTION(BlueprintCallable)
	TSubclassOf<class ARunePreviewAgent> GetPreviewAgentClass() const;

public:
	/** Invoked when tried to apply effects */
	UPROPERTY(BlueprintAssignable)
	FAgentApplicationDelegate onApplyEffects;

	/** Invoked when tried to revert effects */
	UPROPERTY(BlueprintAssignable)
	FAgentApplicationDelegate onRevertEffects;

protected:
	/** Time - in seconds - before destroying the agent. If negative, it will last alive until manual destruction. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RuneTangibleAgent: General Settings")
	float duration;

	/** Actor class used to preview the agent. */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "RuneTangibleAgent: General Settings")
	TSubclassOf<class ARunePreviewAgent> previewAgentClass;

	/** Rune effects attached to the tangible agent */
	UPROPERTY(VisibleInstanceOnly, BlueprintReadOnly, Category = "RuneTangibleAgent: Debug Variables")
	TArray<class URuneEffect*> attachedRuneEffects;

private:
	friend class URuneUtils;
};
