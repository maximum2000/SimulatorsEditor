#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Select : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Select(std::vector<std::string>* args);
	private:
		ElementAttribute Names, NeedPush;
	};
}
