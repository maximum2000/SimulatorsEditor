#pragma once
#include <map>
#include <string>
#include <vector>
#include <guiddef.h>
namespace ScenarioEditorScenarioStorage
{
	void ClearScenarioStorage();
	void AddScenarioStorageElement(std::string name, GUID guid);
	void CreateNewScenario();
	std::vector<std::string> GetScenarioNames();
	void SetActualScenario(int Scenario);
	std::string GetActualGUID();
	void SetActualScenarioName(std::string Name);
	void RemoveScenario(std::string GUID);
	void DoubleScenario(std::string GUID);
	std::vector<std::vector<std::string>> GetScenarios();
	void SwapScenario(int index_a, int index_b);
}

