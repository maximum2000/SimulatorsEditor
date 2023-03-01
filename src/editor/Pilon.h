#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Pilon : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Pilon(std::vector<std::string>* args);
		Pilon();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute objectName, x, y, z;
	};
}
