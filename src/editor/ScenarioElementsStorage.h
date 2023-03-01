#pragma once
#include <vector>
#include <string>
#include <memory>
#include "ScenarioElement.h"

namespace ScenariosEditorScenarioElement
{
	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddScenarioElementStorageElement(std::vector<std::string>* args);
	void RemoveScenarioElementStorageElement(std::shared_ptr<ScenarioElement> Element);
	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddScenarioElementStorageElement(std::string ElementName, float x, float y, float alfa, int pin_count);
	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddScenarioElementStorageElement(std::string ElementName, float x, float y, float alfa, int pin_count, std::shared_ptr<ScenarioElement> CopyOrigin);
	void ClearScenarioElementStorage();
	void LoadElements();
	void UpdateCoordinates(std::shared_ptr<ScenarioElement> Elem, float new_x, float new_y);
	void AddScenarioElementStorageLink(std::vector<int> args);
	void RemoveScenarioElementStorageLink(std::vector<int> args);
	int GetPinIndex(const char* name, int pin);
	std::vector<std::shared_ptr<ScenarioElement>> GetScenarioElements();
	std::vector<std::vector<int>> GetLinks();
	std::vector<std::vector<std::string>> GetAllElementsAttributeNames();
}