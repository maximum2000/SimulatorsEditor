#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Outcome : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Outcome(std::vector<std::string>* args);
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute messageToReport, isError, causeToReport, LossesHealthAndDead, LossesEcology,
			LossesEconomic, LossesOther, Detection, Diagnostics, MakingDecisions, ActionsPerformed;
	};
}
