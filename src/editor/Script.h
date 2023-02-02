#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Script : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Script(std::vector<std::string>* args);
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute function, argumentFloat, argumentString;
	};
}