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
}