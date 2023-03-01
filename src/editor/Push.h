#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Push : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Push(std::vector<std::string>* args);
		Push();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute Names;
	};
}

