#include "Select.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Select::ElementName = "select";
	Select::Select(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Names = SetAttributeFromSource(args, 0);
		NeedPush = SetAttributeFromSource(args, 1);
	}
}