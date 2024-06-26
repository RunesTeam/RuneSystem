

#pragma once

#include "CoreMinimal.h"
#include "UObject/UObjectHash.h"
#include "UObject/UObjectGlobals.h"
#include "Kismet/GameplayStatics.h"
#include "RuneBehaviour.h"
#include "RuneTangibleAgent.h"
#include "RunePreviewAgent.h"
#include "RuneCompatible.h"
#include "RuneUtils.generated.h"

UCLASS()
class URuneUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()
	
public:
	/**
	 * Applies an effect to a given target actor.
	 *
	 * @param effect Effect to be applied
	 * @param instigator Controller that will spawn and/or control the causer
	 * @param causer Actor which will apply the effect application.
	 * @param target Actor which will recieve the effect application.
	 * @return Whether or not it was possible to apply the effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Rune Effect", meta = (DefaultToSelf = "effect", HideSelfPin))
	static bool ApplyEffect(class URuneEffect* effect, AController* instigator, AActor* causer, AActor* target);

	/**
	 * Reverts an effect of a given target actor.
	 *
	 * @param effect Effect to be reverted
	 * @param target Actor which will recieve the effect "undo".
	 * @return Whether or not it was possible to revert the effect
	 */
	UFUNCTION(BlueprintCallable, Category = "Rune System|Rune Effect", meta = (DefaultToSelf = "effect", HideSelfPin))
	static bool RevertEffect(class URuneEffect* effect, AActor* target);

	template <class T, typename... Args>
	static T* SpawnTangibleAgent(const URuneBehaviour& behaviour, UClass* InClass, Args... args);

	template <class T, typename... Args>
	static T* SpawnTangibleAgent(const URuneBehaviour& behaviour, const FRuneTangibleAgentTemplate& agentTemplate, Args... args);

	template <class T, typename... Args>
	static T* SpawnPreviewAgent(const URuneBehaviour& behaviour, UClass* InClass, Args... args);

	template <class T, typename... Args>
	static T* SpawnPreviewAgent(const URuneBehaviour& behaviour, const FRuneTangibleAgentTemplate& agentTemplate, Args... args);

};

template <class T, typename... Args>
static T* URuneUtils::SpawnTangibleAgent(const URuneBehaviour& behaviour, UClass* InClass, Args... args)
{
	UClass* TClass = T::StaticClass();
	if (!TClass->IsChildOf(ARuneTangibleAgent::StaticClass()) && TClass != ARuneTangibleAgent::StaticClass())
	{
		return nullptr;
	}

	// check if InClass is NOT a ARuneTangibleAgent
	if (!InClass->IsChildOf(ARuneTangibleAgent::StaticClass()) && InClass != ARuneTangibleAgent::StaticClass())
	{
		return nullptr;
	}

	// check world existance
	UWorld* world = behaviour.GetWorld();
	if (world == nullptr)
	{
		return nullptr;
	}

	FActorSpawnParameters spawnInfo;
	spawnInfo.bDeferConstruction = true;
	spawnInfo.Owner = behaviour.GetOwner();
	if (behaviour.runeOwner != nullptr)
	{
		if (AController* controller = behaviour.runeOwner->GetController())
		{
			spawnInfo.Instigator = behaviour.runeOwner->GetController()->GetPawn();
		}
	}

	ARuneTangibleAgent* agent = world->SpawnActor<ARuneTangibleAgent>(InClass, spawnInfo);
	if (agent != nullptr)
	{
		behaviour.onTangibleAgentSpawnBegin.Broadcast(agent);

		agent->SetAttachedRuneEffects(behaviour.onApplyPulse.GetAllObjects());
		agent->FinishSpawning(std::forward<Args>(args)...);

		behaviour.onTangibleAgentSpawnEnd.Broadcast(agent);
	}
	return CastChecked<T>(agent, ECastCheckedType::NullAllowed);
}

template <class T, typename... Args>
static T* URuneUtils::SpawnTangibleAgent(const URuneBehaviour& behaviour, const FRuneTangibleAgentTemplate& agentTemplate, Args... args)
{
	UClass* TClass = T::StaticClass();
	if (!TClass->IsChildOf(ARuneTangibleAgent::StaticClass()) && TClass != ARuneTangibleAgent::StaticClass())
	{
		return nullptr;
	}

	// check if InClass is NOT a ARuneTangibleAgent
	if (!agentTemplate.agentClass->IsChildOf(ARuneTangibleAgent::StaticClass()) && agentTemplate.agentClass != ARuneTangibleAgent::StaticClass())
	{
		return nullptr;
	}

	// check world existance
	UWorld* world = behaviour.GetWorld();
	if (world == nullptr)
	{
		return nullptr;
	}

	FActorSpawnParameters spawnInfo;
	spawnInfo.bDeferConstruction = true;
	spawnInfo.Owner = behaviour.GetOwner();
	if (behaviour.runeOwner != nullptr)
	{
		if (AController* controller = behaviour.runeOwner->GetController())
		{
			spawnInfo.Instigator = behaviour.runeOwner->GetController()->GetPawn();
		}
	}

	ARuneTangibleAgent* agent = world->SpawnActor<ARuneTangibleAgent>(agentTemplate.agentClass, spawnInfo);
	if (agent != nullptr)
	{
		// initialize templated properties
		for (auto& propPair : agentTemplate.properties)
		{
			FProperty* prop = FindFProperty<FProperty>(agent->GetClass(), propPair.Key);
			if (prop == nullptr) continue;

			prop->ImportText(*propPair.Value, prop->ContainerPtrToValuePtr<uint8>(agent, 0), PPF_None, agent);
		}

		// invoked after setting properties to have consitent data
		behaviour.onTangibleAgentSpawnBegin.Broadcast(agent);

		agent->SetAttachedRuneEffects(behaviour.onApplyPulse.GetAllObjects());
		agent->FinishSpawning(std::forward<Args>(args)...);

		behaviour.onTangibleAgentSpawnEnd.Broadcast(agent);
	}

	return CastChecked<T>(agent, ECastCheckedType::NullAllowed);
}

template<class T, typename ...Args>
inline T* URuneUtils::SpawnPreviewAgent(const URuneBehaviour& behaviour, UClass* InClass, Args ...args)
{
	UClass* TClass = T::StaticClass();
	if (!TClass->IsChildOf(ARunePreviewAgent::StaticClass()) && TClass != ARunePreviewAgent::StaticClass())
	{
		return nullptr;
	}

	// check if InClass is NOT a ARunePreviewAgent
	if (!InClass->IsChildOf(ARunePreviewAgent::StaticClass()) && InClass != ARunePreviewAgent::StaticClass())
	{
		return nullptr;
	}

	// check world existance
	UWorld* world = behaviour.GetWorld();
	if (world == nullptr)
	{
		return nullptr;
	}

	ARunePreviewAgent* previewAgent = world->SpawnActorDeferred<ARunePreviewAgent>(InClass, std::forward<Args>(args)...);
	if (previewAgent != nullptr)
	{
		UObject* tangibleAgent = InClass->GetDefaultObject();
		// set tangibleagent defaults then override them with the properties
		for (TFieldIterator<FProperty> PropIt(InClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;

			if (Property->HasAnyPropertyFlags(CPF_Edit)
				&& !Property->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnTemplate | CPF_DisableEditOnInstance))
			{
				FProperty* prop = FindFProperty<FProperty>(previewAgent->GetClass(), Property->GetFName());
				if (prop == nullptr) continue;

				FString serializedData;
				Property->ExportText_InContainer(0, serializedData, tangibleAgent, tangibleAgent, Property->GetOwnerUObject(), PPF_None);

				prop->ImportText(*serializedData, prop->ContainerPtrToValuePtr<uint8>(previewAgent, 0), PPF_None, previewAgent);
			}
		}

		previewAgent->isInitializedHidden ? previewAgent->Hide() : previewAgent->Show();

		// invoked after setting properties to have consitent data
		behaviour.onPreviewAgentSpawnBegin.Broadcast(previewAgent);

		previewAgent->FinishSpawning(std::forward<Args>(args)...);

		behaviour.onPreviewAgentSpawnBegin.Broadcast(previewAgent);
	}
	return CastChecked<T>(previewAgent, ECastCheckedType::NullAllowed);
}

template<class T, typename ...Args>
inline T* URuneUtils::SpawnPreviewAgent(const URuneBehaviour& behaviour, const FRuneTangibleAgentTemplate& agentTemplate, Args ...args)
{
	UClass* TClass = T::StaticClass();
	if (!TClass->IsChildOf(ARunePreviewAgent::StaticClass()) && TClass != ARunePreviewAgent::StaticClass())
	{
		return nullptr;
	}

	// check if InClass is NOT a ARuneTangibleAgent
	if (!agentTemplate.agentClass->IsChildOf(ARuneTangibleAgent::StaticClass()) && agentTemplate.agentClass != ARuneTangibleAgent::StaticClass())
	{
		return nullptr;
	}

	UClass* previewClass = nullptr;

	FName previewAgentClassPropertyName = GET_MEMBER_NAME_CHECKED(ARuneTangibleAgent, previewAgentClass);
	if (agentTemplate.properties.Contains(previewAgentClassPropertyName))
	{
		FString className = agentTemplate.properties[previewAgentClassPropertyName];
		if (className == FString::Printf(TEXT("None"))) return nullptr;

		// find class and instanciated it
		UObject* classPackage = ANY_PACKAGE;
		if (UClass* result = FindObject<UClass>(classPackage, *className))
		{
			previewClass = result;
		}
		else if (UObjectRedirector* renamedClassRedirector = FindObject<UObjectRedirector>(classPackage, *className))
		{
			previewClass = CastChecked<UClass>(renamedClassRedirector->DestinationObject);
		}
	}
	else
	{
		// get default object to get its preview
		ARuneTangibleAgent* defaultTangibleAgent = Cast<ARuneTangibleAgent>(agentTemplate.agentClass->GetDefaultObject());
		if (defaultTangibleAgent == nullptr)
		{
			return nullptr;
		}

		TSubclassOf<ARunePreviewAgent> previewAgentClass = defaultTangibleAgent->GetPreviewAgentClass();
		if (previewAgentClass == nullptr)
		{
			return nullptr;
		}

		previewClass = previewAgentClass;
	}

	if (previewClass == nullptr)
	{
		return nullptr;
	}

	// check world existance
	UWorld* world = behaviour.GetWorld();
	if (world == nullptr)
	{
		return nullptr;
	}

	FActorSpawnParameters spawnInfo;
	spawnInfo.bDeferConstruction = true;

	ARunePreviewAgent* previewAgent = world->SpawnActor<ARunePreviewAgent>(previewClass, spawnInfo);
	if (previewAgent != nullptr)
	{
		UObject* tangibleAgent = agentTemplate.agentClass->GetDefaultObject();
		// set tangibleagent defaults then override them with the properties
		for (TFieldIterator<FProperty> PropIt(agentTemplate.agentClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
		{
			FProperty* Property = *PropIt;

			if (Property->HasAnyPropertyFlags(CPF_Edit)
				&& !Property->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnTemplate | CPF_DisableEditOnInstance))
			{
				FProperty* prop = FindFProperty<FProperty>(previewAgent->GetClass(), Property->GetFName());
				if (prop == nullptr) continue;

				FString serializedData;
				Property->ExportText_InContainer(0, serializedData, tangibleAgent, tangibleAgent, Property->GetOwnerUObject(), PPF_None);

				prop->ImportText(*serializedData, prop->ContainerPtrToValuePtr<uint8>(previewAgent, 0), PPF_None, previewAgent);
			}
		}

		// initialize templated properties
		for (auto& propPair : agentTemplate.properties)
		{
			FProperty* prop = FindFProperty<FProperty>(previewAgent->GetClass(), propPair.Key);
			if (prop == nullptr) continue;

			prop->ImportText(*propPair.Value, prop->ContainerPtrToValuePtr<uint8>(previewAgent, 0), PPF_None, previewAgent);
		}

		previewAgent->isInitializedHidden ? previewAgent->Hide() : previewAgent->Show();

		// invoked after setting properties to have consitent data
		behaviour.onPreviewAgentSpawnBegin.Broadcast(previewAgent);

		previewAgent->FinishSpawning(std::forward<Args>(args)...);

		behaviour.onPreviewAgentSpawnEnd.Broadcast(previewAgent);
	}

	return CastChecked<T>(previewAgent, ECastCheckedType::NullAllowed);
}

// ----------------------------
// Extendable Enums
// ----------------------------

template <typename DataType>
concept ExtendableEnumConcept = std::is_enum_v<DataType> && requires()
{
	DataType::FIRST_ENUM;
	DataType::LAST_ENUM;
};

template <ExtendableEnumConcept BaseEnum>
struct EnumExtension
{
	using ExtensionType = void; //Default to no extension
};

template <ExtendableEnumConcept BaseEnum>
using EnumExtensionT = typename EnumExtension<BaseEnum>::ExtensionType;


template <typename E>
constexpr typename std::underlying_type<E>::type to_underlying(E e) noexcept {
	return static_cast<typename std::underlying_type<E>::type>(e);
};

template <ExtendableEnumConcept Base, ExtendableEnumConcept Extension>
consteval bool Can_ExtendEnum()
{
	//Must have same underlying type, and ranges cannot overlap
	return std::is_same_v<std::underlying_type_t<Base>,
		std::underlying_type_t<Extension>> &&
		(to_underlying(Base::LAST_ENUM) < to_underlying(Extension::FIRST_ENUM));
}

template <ExtendableEnumConcept Base, ExtendableEnumConcept Extension>
consteval bool Get_IsEnumExtension()
{
	if (!Can_ExtendEnum<Base, Extension>())
		return false; //Not even possible (e.g. range overlap)

	using NextExtension = EnumExtensionT<Base>;
	if (std::is_same_v<NextExtension, Extension>)
		return true; //Found it!

	if constexpr (std::is_same_v<NextExtension, void>)
		return false; //Reached the end of the chain: not an extension
	else //Go to the next link in the extension chain
		return Get_IsEnumExtension<NextExtension, Extension>();
}

template <ExtendableEnumConcept BaseEnum>
class ExtendedEnumBase
{
protected:
	using DataType = std::underlying_type_t<BaseEnum>;
	DataType mValue = std::numeric_limits<DataType>::max();
public:
	constexpr ExtendedEnumBase() = default;
	constexpr ExtendedEnumBase(DataType aValue) : mValue(aValue) {};
};

template <ExtendableEnumConcept BaseEnum>
class ExtendedEnum : public ExtendedEnumBase<BaseEnum>
{
public:
	using DataType = typename ExtendedEnumBase<BaseEnum>::DataType;

	constexpr DataType Get() const { return this->mValue; }

	template <ExtendableEnumConcept EnumType>
	constexpr EnumType Get() const
		requires (Is_Storable<EnumType>())
	{
		return EnumType(this->mValue);
	}

	constexpr operator DataType() const { return Get(); }
public:
	//...
	//IS_STORABLE:
	template <ExtendableEnumConcept EnumType>
	static constexpr bool Is_Storable()
		requires(std::is_same_v<EnumType, BaseEnum>)
	{
		return true;
	}

	template <ExtendableEnumConcept EnumType>
	static constexpr bool Is_Storable()
		requires(!std::is_same_v<EnumType, BaseEnum>)
	{
		return (Get_IsEnumExtension<BaseEnum, EnumType>());
	}

	//CONSTRUCTORS
	constexpr ExtendedEnum() = default;

	template <ExtendableEnumConcept EnumType>
	constexpr ExtendedEnum(EnumType aEnum)
		requires (Is_Storable<EnumType>()) : ExtendedEnumBase<BaseEnum>{ to_underlying(aEnum) } {}

	//ASSIGNMENT
	template <ExtendableEnumConcept EnumType>
	constexpr ExtendedEnum& operator=(EnumType aEnum)
		requires (Is_Storable<EnumType>())
	{
		this->mValue = to_underlying(aEnum); return *this;
	}

	template <ExtendableEnumConcept EnumType>
	constexpr bool operator==(EnumType aEnum) const
		requires (Is_Storable<EnumType>())
	{
		return Get() == to_underlying(aEnum);
	}
};

#define EXTEND_ENUM(BaseEnum, Enum)	template <> \
									struct EnumExtension<BaseEnum> { using ExtensionType = Enum; };
