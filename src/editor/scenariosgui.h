#pragma once
#include "ScenarioElement.h"
#include <memory>

namespace ScenariosEditorGUI {
	void ShowDemoScenarioGUI();
	void AddElement(const char* name, float x, float y, std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> actual_element);
	void AddLink(int element_a_index, int element_b_index, int element_a_point, int element_b_point);
	const char* GetNameOfElementOnCanvas(int index);
	void AddScenarioElementStoragePin(std::vector<int> args);
	int GetNumOfElement(std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> Elem);
	void ClearElements();
}