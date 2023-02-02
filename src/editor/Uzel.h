#pragma once
#include "ScenarioElement.h"
#include <iostream> // delete later
namespace ScenariosEditorScenarioElement
{
	class Uzel : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Uzel(std::vector<std::string>* args);
		std::vector<ElementAttribute*> GetAttributes();
	private:
	};
}