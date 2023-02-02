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
	std::vector<ElementAttribute*> Arrow::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&CenterObjectName);
		ret.emplace_back(&LookAtObjectName);
		ret.emplace_back(&NeedPush);
		return ret;
	}
}