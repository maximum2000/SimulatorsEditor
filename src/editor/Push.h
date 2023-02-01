#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Push : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Push(std::vector<std::string>* args);
	private:
		ElementAttribute Names;
	};
}

