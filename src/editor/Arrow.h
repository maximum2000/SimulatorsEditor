#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Arrow : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Arrow(std::vector<std::string>* args);
	private:
		ElementAttribute CenterObjectName, LookAtObjectName, NeedPush;
	};
}

