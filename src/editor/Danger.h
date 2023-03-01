#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Danger : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Danger(std::vector<std::string>* args);
		Danger();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute gas, object, distance_to_object;
	};
}
