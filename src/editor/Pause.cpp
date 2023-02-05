#include "Pause.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Pause::ElementName = "pause";
	Pause::Pause(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Duration = SetAttributeFromSource(args, 2);
	}
	Pause::Pause() : ScenarioElement::ScenarioElement()
	{
		Duration = ElementAttribute(u8"Duration",2);
	}
	std::vector<ElementAttribute*> Pause::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Duration);
		return ret;
	}
}