#include "Danger.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Danger::ElementName = "danger";
	Danger::Danger(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		gas = SetAttributeFromSource(args, 2);
		object = SetAttributeFromSource(args, 0);
		distance_to_object = SetAttributeFromSource(args, 2);
	}
	Danger::Danger() : ScenarioElement::ScenarioElement()
	{
		gas = ElementAttribute::ElementAttribute(u8"gas",2);
		object = ElementAttribute::ElementAttribute(u8"object",0);
		distance_to_object = ElementAttribute::ElementAttribute(u8"distance_to_object",2);
	}
	std::vector<ElementAttribute*> Danger::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&gas);
		ret.emplace_back(&object);
		ret.emplace_back(&distance_to_object);
		return ret;
	}
}