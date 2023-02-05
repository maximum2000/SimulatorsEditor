#include "Message.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Message::ElementName = "message";
	Message::Message(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Text = SetAttributeFromSource(args, 0);
		WithButtonOk = SetAttributeFromSource(args, 1);
	}
	Message::Message() : ScenarioElement::ScenarioElement()
	{
		Text = ElementAttribute(u8"Text",0);
		WithButtonOk = ElementAttribute(u8"WithButtonOk",1);
	}
	std::vector<ElementAttribute*> Message::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Text);
		ret.emplace_back(&WithButtonOk);
		return ret;
	}
}