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
	Outcome::Outcome() : ScenarioElement::ScenarioElement()
	{
		messageToReport = ElementAttribute(u8"messageToReport",0);
		isError = ElementAttribute(u8"isError",1);
		causeToReport = ElementAttribute(u8"causeToReport",0);
		LossesHealthAndDead = ElementAttribute(u8"LossesHealthAndDead",2);
		LossesEcology = ElementAttribute(u8"LossesEcology",2);
		LossesEconomic = ElementAttribute(u8"LossesEconomic",2);
		LossesOther = ElementAttribute(u8"LossesOther",2);
		Detection = ElementAttribute(u8"Detection",1);
		Diagnostics = ElementAttribute(u8"Diagnostics",1);
		MakingDecisions = ElementAttribute(u8"MakingDecisions",1);
		ActionsPerformed = ElementAttribute(u8"ActionsPerformed",1);
	}
	std::vector<ElementAttribute*> Outcome::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&messageToReport);
		ret.emplace_back(&isError);
		ret.emplace_back(&causeToReport);
		ret.emplace_back(&LossesHealthAndDead);
		ret.emplace_back(&LossesEcology);
		ret.emplace_back(&LossesEconomic);
		ret.emplace_back(&LossesOther);
		ret.emplace_back(&Detection);
		ret.emplace_back(&Diagnostics);
		ret.emplace_back(&MakingDecisions);
		ret.emplace_back(&ActionsPerformed);
		return ret;
	}
}