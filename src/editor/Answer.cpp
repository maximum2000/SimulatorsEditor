#include "Answer.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Answer::ElementName = "answer";
	Answer::Answer(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Question = SetAttributeFromSource(args, 0);
		Answer1 = SetAttributeFromSource(args, 0);
		Answer2 = SetAttributeFromSource(args, 0);
		Answer3 = SetAttributeFromSource(args, 0);
		Answer4 = SetAttributeFromSource(args, 0);
		MyActionsButton = SetAttributeFromSource(args, 1);
	}
	std::vector<ElementAttribute*> Answer::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Question);
		ret.emplace_back(&Answer1);
		ret.emplace_back(&Answer2);
		ret.emplace_back(&Answer3);
		ret.emplace_back(&Answer4);
		ret.emplace_back(&MyActionsButton);
		return ret;
	}
}