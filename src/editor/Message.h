#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Message : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Message(std::vector<std::string>* args);
	private:
		ElementAttribute Text, WithButtonOk;
	};
}

