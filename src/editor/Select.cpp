#include "Select.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Select::ElementName = "select";
	Select::Select(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Names = SetAttributeFromSource(args, 0);
		NeedPush = SetAttributeFromSource(args, 1);
	}
	std::vector<ElementAttribute*> Select::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Names);
		ret.emplace_back(&NeedPush);
		return ret;
	}
}