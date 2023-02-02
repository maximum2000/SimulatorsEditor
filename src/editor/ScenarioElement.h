#pragma once
#include <string>
#include <vector>
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class ScenarioElement
	{
	public:
		virtual std::vector<ElementAttribute*> GetAttributes() = 0;
		virtual ~ScenarioElement() {}
		ScenarioElement(std::vector<std::string>* args);
		std::string caption;
	protected:
		std::string ScenarioGUID;
		float x, y, alfa;
		std::vector<int> pins;
	};
}

