#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class VariableValue : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		VariableValue(std::vector<std::string>* args);
		VariableValue();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute variable_X, variable_A, variable_fix_A, variable_B, variable_fix_B;
	};
}