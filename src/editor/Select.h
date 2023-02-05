#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Select : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Select(std::vector<std::string>* args);
		Select();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute Names, NeedPush;
	};
}
