#include "Pilon.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Pilon::ElementName = "pilon";
	Pilon::Pilon(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		objectName = SetAttributeFromSource(args, 0);
		x = SetAttributeFromSource(args, 2);
		y = SetAttributeFromSource(args, 2);
		z = SetAttributeFromSource(args, 2);
	}
	Pilon::Pilon() : ScenarioElement::ScenarioElement()
	{
		objectName = ElementAttribute(u8"objectName",0);
		x = ElementAttribute(u8"x",2);
		y = ElementAttribute(u8"y",2);
		z = ElementAttribute(u8"z",2);
	}
	std::vector<ElementAttribute*> Pilon::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&objectName);
		ret.emplace_back(&x);
		ret.emplace_back(&y);
		ret.emplace_back(&z);
		return ret;
	}
}