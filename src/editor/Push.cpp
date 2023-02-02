#include "Push.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Push::ElementName = "push";
	Push::Push(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Names = SetAttributeFromSource(args, 0);
	}
	std::vector<ElementAttribute*> Push::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Names);
		return ret;
	}
}