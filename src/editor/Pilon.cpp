#include "Pilon.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Pilon::ElementName = "pilon";
	Pilon::Pilon(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		objectName = SetAttributeFromSource(args, 0);
		x = SetAttributeFromSource(args, 2);
		y = SetAttributeFromSource(args, 2);
		z = SetAttributeFromSource(args, 2);
	}
}