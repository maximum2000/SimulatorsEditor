#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Arrow : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Arrow(std::vector<std::string>* args);
		Arrow();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute CenterObjectName, LookAtObjectName, NeedPush;
	};
}

