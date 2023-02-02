#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Pilon : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Pilon(std::vector<std::string>* args);
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute objectName, x, y, z;
	};
}
