#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Clear : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Clear(std::vector<std::string>* args);
		Clear();
		std::vector<ElementAttribute*> GetAttributes();
	private:
	};
}