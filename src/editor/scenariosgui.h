#pragma once
#include "ScenarioElement.h"
#include <memory>
#include <vector>
#include "imgui.h"

namespace ScenariosEditorGUI {
	// Used only outside of scenariosgui.cpp
	void ShowDemoScenarioGUI();
	void AddElement(const char* name, float x, float y, std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> actual_element);
	void AddLink(int element_a_index, int element_b_index, int element_a_point, int element_b_point);
	const char* GetNameOfElementOnCanvas(int index);

	// Used inside and outside of scenariosgui.cpp
	int GetNumOfElement(std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> Elem);
	void ClearElements();

}