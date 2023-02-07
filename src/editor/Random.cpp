#include "Random.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Random::ElementName = "random";
	Random::Random(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		need = SetAttributeFromSource(args, 2);
	}
	Random::Random() : ScenarioElement::ScenarioElement()
	{
		need = ElementAttribute(u8"need",2);
	}
	std::vector<ElementAttribute*> Random::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&need);
		return ret;
	}
}