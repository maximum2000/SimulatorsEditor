#pragma once
#include <map>
#include <string>
#include <vector>
#include <guiddef.h>
namespace ScenarioEditorScenarioStorage
{
	void ClearScenarioStorage();
	void AddScenarioStorageElement(std::string name, GUID guid);
	std::vector<std::string> GetScenarioNames();
	void SetActualScenario(int Scenario);
	std::string GetActualGUID();
	std::vector<std::vector<std::string>> GetScenarios();
}

