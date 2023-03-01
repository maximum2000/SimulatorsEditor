#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Pause : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Pause(std::vector<std::string>* args);
		Pause();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute Duration;
	};
}
