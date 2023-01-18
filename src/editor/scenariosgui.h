#pragma once
namespace ScenariosEditorGUI {
	void ShowDemoScenarioGUI();
	void AddElement(const char* name, float x, float y);
	void AddLink(int element_a_index, int element_b_index, int element_a_point, int element_b_point);
	const char* GetElementName(int index);
}