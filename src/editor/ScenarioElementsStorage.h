#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddElement(std::vector<std::string>* args);
}