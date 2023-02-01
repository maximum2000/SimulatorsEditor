#include "Outcome.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Outcome::ElementName = "outcome";
	Outcome::Outcome(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		messageToReport = SetAttributeFromSource(args, 0);
		isError = SetAttributeFromSource(args, 1);
		causeToReport = SetAttributeFromSource(args, 0);
		LossesHealthAndDead = SetAttributeFromSource(args, 2);
		LossesEcology = SetAttributeFromSource(args, 2);
		LossesEconomic = SetAttributeFromSource(args, 2);
		LossesOther = SetAttributeFromSource(args, 2);
		Detection = SetAttributeFromSource(args, 1);
		Diagnostics = SetAttributeFromSource(args, 1);
		MakingDecisions = SetAttributeFromSource(args, 1);
		ActionsPerformed = SetAttributeFromSource(args, 1);
	}
}