#include "Select.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Select::ElementName = "select";
	Select::Select(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Names = SetAttributeFromSource(args, 0);
		NeedPush = SetAttributeFromSource(args, 1);
	}
	Select::Select() : ScenarioElement::ScenarioElement()
	{
		Names = ElementAttribute(u8"Names",0);
		NeedPush = ElementAttribute(u8"NeedPush",1);
	}
	std::vector<ElementAttribute*> Select::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Names);
		ret.emplace_back(&NeedPush);
		return ret;
	}
}