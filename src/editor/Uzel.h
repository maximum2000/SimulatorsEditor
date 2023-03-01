#pragma once
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	class Uzel : public ScenarioElement
	{
	public:
		const static std::string ElementName; std::string getElementName() { return ElementName; }
		Uzel(std::vector<std::string>* args);
		Uzel();
		std::vector<ElementAttribute*> GetAttributes();
	private:
	};
}