#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	ScenarioElement::ScenarioElement(std::vector<std::string>* args)
	{
		ScenarioGUID = (*args)[1];
		x = std::stof((*args)[2]);
		y = std::stof((*args)[3]);
		alfa = std::stof((*args)[4]);
		int i = 5;
		while ((*args)[i] != "Separator_Pins")
		{
			pins.push_back(std::stof((*args)[i++]));
		}
		caption = (*args)[++i];
		(*args).erase((*args).begin(), (*args).begin() + ++i);
	}
	ScenarioElement::ScenarioElement()
	{
		ScenarioGUID = "";
		x = 0.00f;
		y = 0.00f;
		alfa = 0.00f;
		pins = std::vector<int>();
		caption = "";
	}
}