#include "Script.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Script::ElementName = "script";
	Script::Script(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		function = SetAttributeFromSource(args, 0);
		argumentFloat = SetAttributeFromSource(args, 2);
		argumentString = SetAttributeFromSource(args, 0);
	}
	std::vector<ElementAttribute*> Script::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&function);
		ret.emplace_back(&argumentFloat);
		ret.emplace_back(&argumentString);
		return ret;
	}
}