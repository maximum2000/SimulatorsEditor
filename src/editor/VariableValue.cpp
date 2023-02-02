#include "VariableValue.h"
namespace ScenariosEditorScenarioElement
{
	const std::string VariableValue::ElementName = "variable_value";
	VariableValue::VariableValue(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		variable_X = SetAttributeFromSource(args, 0);
		variable_A = SetAttributeFromSource(args, 0);
		variable_fix_A = SetAttributeFromSource(args, 0);
		variable_B = SetAttributeFromSource(args, 0);
		variable_fix_B = SetAttributeFromSource(args, 0);
	}
	std::vector<ElementAttribute*> VariableValue::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&variable_X);
		ret.emplace_back(&variable_A);
		ret.emplace_back(&variable_fix_A);
		ret.emplace_back(&variable_B);
		ret.emplace_back(&variable_fix_B);
		return ret;
	}
}