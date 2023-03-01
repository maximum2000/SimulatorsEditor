#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Script : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Script(std::vector<std::string>* args);
		Script();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute function, argumentFloat, argumentString;
	};
}