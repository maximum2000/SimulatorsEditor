#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Pause : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Pause(std::vector<std::string>* args);
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute Duration;
	};
}
