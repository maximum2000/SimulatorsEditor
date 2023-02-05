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
	VariableValue::VariableValue() : ScenarioElement::ScenarioElement()
	{
		variable_X = ElementAttribute(u8"variable_X",0);
		variable_A = ElementAttribute(u8"variable_A",0);
		variable_fix_A = ElementAttribute(u8"variable_fix_A",0);
		variable_B = ElementAttribute(u8"variable_B",0);
		variable_fix_B = ElementAttribute(u8"variable_fix_B",0);
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