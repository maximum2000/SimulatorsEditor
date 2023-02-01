#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Sound : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Sound(std::vector<std::string>* args);
	private:
		ElementAttribute Name;
	};
}