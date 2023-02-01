#pragma once
#include <string>
#include <vector>

namespace ScenariosEditorScenarioElement
{
	class ScenarioElement
	{
	public:
		virtual ~ScenarioElement() {}
		ScenarioElement(std::vector<std::string>* args);
	protected:
		std::string ScenarioGUID, caption;
		float x, y, alfa;
		std::vector<int> pins;
	};
}

