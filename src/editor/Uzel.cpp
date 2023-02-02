#include "Uzel.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Uzel::ElementName = "uzel";
	Uzel::Uzel(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{

	}
	std::vector<ElementAttribute*> Uzel::GetAttributes()
	{
		return std::vector<ElementAttribute*>();
	}
}