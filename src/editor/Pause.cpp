#include "Pause.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Pause::ElementName = "pause";
	Pause::Pause(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Duration = SetAttributeFromSource(args, 2);
	}
}