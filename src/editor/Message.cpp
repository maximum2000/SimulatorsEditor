#include "Message.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Message::ElementName = "message";
	Message::Message(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Text = SetAttributeFromSource(args, 0);
		WithButtonOk = SetAttributeFromSource(args, 1);
	}
	std::vector<ElementAttribute*> Message::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Text);
		ret.emplace_back(&WithButtonOk);
		return ret;
	}
}