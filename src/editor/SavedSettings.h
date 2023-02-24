#pragma once
#include <string>
#include <vector>
namespace ScenarioEditorSavedSettings
{
	void Initialization();
	void AddToRecentFiles(std::wstring Path);
	std::vector<std::string> GetRecentFiles();
}