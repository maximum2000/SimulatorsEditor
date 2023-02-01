#include "Random.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Random::ElementName = "random";
	Random::Random(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		need = SetAttributeFromSource(args, 2);
	}
}