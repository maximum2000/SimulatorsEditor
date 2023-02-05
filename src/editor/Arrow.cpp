#include "Arrow.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Arrow::ElementName = "arrow";
	Arrow::Arrow(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		CenterObjectName = SetAttributeFromSource(args, 0);
		LookAtObjectName = SetAttributeFromSource(args, 0);
		NeedPush = SetAttributeFromSource(args, 1);
	}
	Arrow::Arrow() : ScenarioElement::ScenarioElement()
	{
		CenterObjectName = ElementAttribute(u8"CenterObjectName",0);
		LookAtObjectName = ElementAttribute(u8"LookAtObjectName",0);
		NeedPush = ElementAttribute(u8"NeedPush",1);
	}
	std::vector<ElementAttribute*> Arrow::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&CenterObjectName);
		ret.emplace_back(&LookAtObjectName);
		ret.emplace_back(&NeedPush);
		return ret;
	}
}