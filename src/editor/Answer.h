#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Answer : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Answer(std::vector<std::string>* args);
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute Question, Answer1, Answer2, Answer3, Answer4, MyActionsButton;
	};
}