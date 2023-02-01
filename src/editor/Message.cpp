#include "Message.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Message::ElementName = "message";
	Message::Message(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Text = SetAttributeFromSource(args, 0);
		WithButtonOk = SetAttributeFromSource(args, 1);
	}
}