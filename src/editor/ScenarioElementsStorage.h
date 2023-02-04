#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ScenarioElement.h"
#include "ScenarioStorage.h"


namespace ScenariosEditorScenarioElement
{
	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddScenarioElementStorageElement(std::vector<std::string>* args);
	void ClearScenarioElementStorage();
	void LoadElements();
	void AddScenarioElementStorageLink(std::vector<int> args);
}