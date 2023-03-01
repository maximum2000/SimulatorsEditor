#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Random : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Random(std::vector<std::string>* args);
		Random();
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute need;
	};
}