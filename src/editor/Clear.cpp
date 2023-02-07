#include "Clear.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Clear::ElementName = "clear";
	Clear::Clear(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{

	}
	Clear::Clear() : ScenarioElement::ScenarioElement()
	{

	}
	std::vector<ElementAttribute*> Clear::GetAttributes()
	{
		return std::vector<ElementAttribute*>();
	}
}