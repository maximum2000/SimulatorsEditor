#pragma once
#include "ScenarioElement.h"
namespace ScenariosEditorScenarioElement
{
	class Clear : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Clear(std::vector<std::string>* args);
	private:
	};
}