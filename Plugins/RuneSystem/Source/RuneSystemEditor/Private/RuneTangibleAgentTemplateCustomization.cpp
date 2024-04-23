


#include "RuneTangibleAgentTemplateCustomization.h"

#include "IDetailChildrenBuilder.h"
#include "IDetailPropertyRow.h"
#include "DetailWidgetRow.h"
#include "PropertyHandle.h"
#include "Layout/Visibility.h"
#include "UObject/UObjectHash.h"
#include "PropertyCustomizationHelpers.h"
#include "EditorStyleSet.h"

#include "SlateBasics.h"
#include "Utils/RuneTypes.h"
#include "RuneTangibleAgent.h"


TSharedRef<IPropertyTypeCustomization> FRuneTangibleAgentTemplateCustomization::MakeInstance()
{
	// create the instance and returned a SharedRef
	return MakeShareable(new FRuneTangibleAgentTemplateCustomization());
}

void FRuneTangibleAgentTemplateCustomization::CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// cache struct property handle
	structHandle = StructPropertyHandle;

	// get the property handle of the agent class property
	TSharedPtr<IPropertyHandle> agentClassPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRuneTangibleAgentTemplate, agentClass));
	check(agentClassPropertyHandle.IsValid());

	// get the propertu handle of the instanced agent
	agentTemplatePropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRuneTangibleAgentTemplate, agentTemplateObject));
	check(agentTemplatePropertyHandle.IsValid());

	// show property name (not sure how it works with ShowOnlyInnerProperties)
	HeaderRow.NameContent()
		[
			StructPropertyHandle->CreatePropertyNameWidget()
		]
		.ValueContent()
		[
			SNew(SHorizontalBox)
			+ SHorizontalBox::Slot()
			.AutoWidth()
			[
				agentClassPropertyHandle->CreatePropertyValueWidget()
			]
		];

	// update value if needed
	OnAgentClassChanged(agentClassPropertyHandle);

	// initialize data values with properties
	FRuneTangibleAgentTemplate* data = GetData();
	if (data->agentTemplateObject != nullptr)
	{
		for (auto& propPair : data->properties)
		{
			FProperty* prop = FindFProperty<FProperty>(data->agentTemplateObject->GetClass(), propPair.Key);
			if (prop == nullptr) continue;

			prop->ImportText(*propPair.Value, prop->ContainerPtrToValuePtr<uint8>(data->agentTemplateObject, 0), PPF_None, data->agentTemplateObject);
		}
	}

	// attached an event when the property value changed
	StructPropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FRuneTangibleAgentTemplateCustomization::OnAgentClassChanged, agentClassPropertyHandle));
	
	StructPropertyHandle->SetOnChildPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FRuneTangibleAgentTemplateCustomization::OnAgentClassChanged, agentClassPropertyHandle));

	agentClassPropertyHandle->SetOnPropertyValueChanged(
		FSimpleDelegate::CreateSP(this, &FRuneTangibleAgentTemplateCustomization::OnAgentClassChanged, agentClassPropertyHandle));

}

void FRuneTangibleAgentTemplateCustomization::CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils)
{
	// cache struct property handle
	structHandle = StructPropertyHandle;

	TSharedPtr<IPropertyHandle> AgentClassPropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRuneTangibleAgentTemplate, agentClass));
	TSharedPtr<IPropertyHandle> AgentTemplatePropertyHandle = StructPropertyHandle->GetChildHandle(GET_MEMBER_NAME_CHECKED(FRuneTangibleAgentTemplate, agentTemplateObject));

	check(AgentClassPropertyHandle.IsValid());
	check(AgentTemplatePropertyHandle.IsValid());

	// add tsubclassof row
	// left here for future reference
	// header dropdown has some visual bugs (none important ones)
	/*StructBuilder.AddProperty(AgentClassPropertyHandle.ToSharedRef())
		.IsEnabled(MakeAttributeLambda([=]
			{
				UObject* outer = GetOuter();
				UObject* outermost = GetOutermost();
				bool isDefaultObject = outer == outer->GetClass()->GetDefaultObject() || !outermost->IsA(UWorld::StaticClass());
				return isDefaultObject;
			}));*/

	FRuneTangibleAgentTemplate* data = GetData();
	UClass* AgentClass = data->agentClass;
	TArray<TSharedPtr<IPropertyHandle>> PropertyHandles;
	for (TFieldIterator<FProperty> PropIt(AgentClass, EFieldIteratorFlags::IncludeSuper); PropIt; ++PropIt)
	{
		FProperty* Property = *PropIt;
		IDetailPropertyRow* row = nullptr;

		if (Property->HasAnyPropertyFlags(CPF_Edit) 
			&& !Property->HasAnyPropertyFlags(CPF_EditConst | CPF_DisableEditOnTemplate | CPF_DisableEditOnInstance))
		{
			TSharedPtr<IPropertyHandle> PropertyHandle = AgentTemplatePropertyHandle->GetChildHandle(Property->GetFName());
			if (!PropertyHandle.IsValid()) continue;

			StructBuilder.AddProperty(PropertyHandle.ToSharedRef());
			
			// usefull for vectors (and maybe structs, not sure)
			PropertyHandle->SetOnChildPropertyValueChanged(
				FSimpleDelegate::CreateSP(this, &FRuneTangibleAgentTemplateCustomization::OnTemplatePropertyChanged, PropertyHandle, Property)
			);

			// usefull for any individial property
			PropertyHandle->SetOnPropertyValueChanged(
				FSimpleDelegate::CreateSP(this, &FRuneTangibleAgentTemplateCustomization::OnTemplatePropertyChanged, PropertyHandle, Property)
			);

			PropertyHandles.Add(PropertyHandle);
		}
	}
}

void FRuneTangibleAgentTemplateCustomization::OnAgentClassChanged(TSharedPtr<IPropertyHandle> propertyHandle)
{
	// if this method was invoked, TSubclassOf dropdown was changed
	if (propertyHandle.IsValid() && propertyHandle->IsValidHandle())
	{
		FRuneTangibleAgentTemplate* data = GetData();

		bool agentClassNullAndTemplateNonNull = data->agentClass == nullptr && data->agentTemplateObject != nullptr;
		bool agentClassNonNullAndTemplateNull = data->agentClass != nullptr && data->agentTemplateObject == nullptr;
		bool agentClassNonNullAndTemplateDiff = data->agentClass != nullptr && data->agentTemplateObject != nullptr && !data->agentTemplateObject->IsA(data->agentClass);
		bool shouldTemplateUpdate = agentClassNullAndTemplateNonNull || agentClassNonNullAndTemplateNull || agentClassNonNullAndTemplateDiff;

		UObject* outer = GetOuter();
		// instantiate an agent (it will only be used as a way of getting properties)
		if (shouldTemplateUpdate)
		{
			ARuneTangibleAgent* prevObject = data->agentTemplateObject;
			FName uniqueName = data->agentClass != nullptr 
				? *FString::Printf(TEXT("%s_TEMPLATE_%s"), *data->agentClass->GetName(), *FGuid::NewGuid().ToString())
				: nullptr;

			// create new templated version of the actor
			// this will only be instanced to have something to modify, we don't care about saving it on disk
			data->agentTemplateObject = data->agentClass != nullptr ? NewObject<ARuneTangibleAgent>(outer, data->agentClass, uniqueName, RF_Transient | RF_DuplicateTransient) : nullptr;
			
			if (data->agentClass == nullptr ||
				(data->agentTemplateObject && prevObject && !data->agentTemplateObject->IsA(prevObject->GetClass())))
			{
				data->properties.Empty();
			}

			if (prevObject != nullptr)
			{
				prevObject->Destroy();
				prevObject->ConditionalBeginDestroy();
			}
			GEngine->ForceGarbageCollection(true);
		}
	}
}

void FRuneTangibleAgentTemplateCustomization::OnTemplatePropertyChanged(TSharedPtr<IPropertyHandle> propertyHandle, FProperty* inProperty)
{
	FRuneTangibleAgentTemplate* data = GetData();

	// at this point, there should be something to modify
	check(data->agentTemplateObject != nullptr)

	FString serializedData;
	inProperty->ExportText_InContainer(0, serializedData, data->agentTemplateObject, data->agentTemplateObject, inProperty->GetOwnerUObject(), PPF_None);

	// we save the stringfied data into que properties map
	// so we can des-serialize it later on (on spawn)
	FString& strData = data->properties.FindOrAdd(inProperty->GetFName(), serializedData);
	strData = serializedData;

	// if identical as its default, then remove it (only safe modified properties)
	UObject* defaultObject = data->agentTemplateObject->GetClass()->GetDefaultObject();
	FString defaultData;
	inProperty->ExportText_InContainer(0, defaultData, defaultObject, defaultObject, inProperty->GetOwnerUObject(), PPF_None);

	// string comparison (maybe not the best way)
	// works fine if it will only be executed in editor
	if (defaultData == serializedData) 
	{
		data->properties.Remove(inProperty->GetFName());
	}
}

FRuneTangibleAgentTemplate* FRuneTangibleAgentTemplateCustomization::GetData()
{
	check(structHandle != nullptr);

	TArray<UObject*> objects;
	structHandle->GetOuterObjects(objects);
	if (objects.Num() == 0) return nullptr;

	return (FRuneTangibleAgentTemplate*)structHandle->GetValueBaseAddress((uint8*)objects[0]);
}

UObject* FRuneTangibleAgentTemplateCustomization::GetOuter() const
{
	check(structHandle != nullptr);

	TArray<UObject*> objects;
	structHandle->GetOuterObjects(objects);
	if (objects.Num() == 0) return nullptr;

	return objects[0];
}

UObject* FRuneTangibleAgentTemplateCustomization::GetOutermost() const
{
	check(structHandle != nullptr);

	TArray<UObject*> objects;
	structHandle->GetOuterObjects(objects);
	if (objects.Num() == 0) return nullptr;

	return objects[0]->GetOutermostObject();
}
