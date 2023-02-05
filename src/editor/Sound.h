#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Sound : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Sound(std::vector<std::string>* args);
		Sound();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute Name;
	};
}