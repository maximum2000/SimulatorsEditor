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
}