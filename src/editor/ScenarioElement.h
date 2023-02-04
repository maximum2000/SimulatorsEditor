#pragma once
#include <string>
#include <vector>
#include "ElementAttribute.h"
#include <iostream>

namespace ScenariosEditorScenarioElement
{
	template<class Type>
	std::string GetName(Type a) {
		return (Type)a.Name;
	}
	class ScenarioElement
	{
	public:
		virtual std::vector<ElementAttribute*> GetAttributes() = 0;
		virtual ~ScenarioElement() { }
		virtual std::string getElementName() = 0;
		ScenarioElement(std::vector<std::string>* args);
		std::string caption;
		std::string ScenarioGUID;
		float x, y, alfa;
		std::vector<int> pins;
	protected:
		
	};
}

