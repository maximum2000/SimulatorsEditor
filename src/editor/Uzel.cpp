#include "Uzel.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Uzel::ElementName = "uzel";
	Uzel::Uzel(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{

	}
	Uzel::Uzel() : ScenarioElement::ScenarioElement()
	{
	}
	std::vector<ElementAttribute*> Uzel::GetAttributes()
	{
		return std::vector<ElementAttribute*>();
	}
}