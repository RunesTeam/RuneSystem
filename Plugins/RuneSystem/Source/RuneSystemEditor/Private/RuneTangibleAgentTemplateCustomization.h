

#pragma once

#include "IPropertyTypeCustomization.h"

/**
 * This PropertyTypeCustomization makes sure that an archetype instance of a TSubclassOf
 * is always available and re-instancing if necesarry already existent instances.
 */
class FRuneTangibleAgentTemplateCustomization : public IPropertyTypeCustomization
{
public:
	/** Makes an instance of teh curernt PropertyTypeCustomization */
	static TSharedRef<IPropertyTypeCustomization> MakeInstance();

	/** Defines how the header of the property should be shown */
	virtual void CustomizeHeader(TSharedRef<IPropertyHandle> StructPropertyHandle, class FDetailWidgetRow& HeaderRow, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;
	
	/** Defines how the children(content) of the property should be shown */
	virtual void CustomizeChildren(TSharedRef<IPropertyHandle> StructPropertyHandle, class IDetailChildrenBuilder& StructBuilder, IPropertyTypeCustomizationUtils& StructCustomizationUtils) override;

protected:
	/** Callback invoked when the TSubclassOf property has changed */
	void OnAgentClassChanged(TSharedPtr<IPropertyHandle> propertyHandle);

	/** Callback invoked when a template property has changed */
	void OnTemplatePropertyChanged(TSharedPtr<IPropertyHandle> propertyHandle, FProperty* inProperty);

	/** Gets a pointer to the data that is been modified */
	struct FRuneTangibleAgentTemplate* GetData();

	/** Gets a pointer to the outer object containing the property that is been modified */
	class UObject* GetOuter() const;

	/** Gets a pointer to the outermost object containing the property that is been modified */
	class UObject* GetOutermost() const;

private:
	/** Main struct handle (used in GetData()) */
	TSharedPtr<IPropertyHandle> structHandle;

	/** Property handle of the TSubclassOf property */
	TSharedPtr<IPropertyHandle> agentTemplatePropertyHandle;
};