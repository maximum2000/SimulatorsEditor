#include "ScenarioElementsStorage.h"
#include <vector>
#include "Uzel.h"
#include "Start.h"
#include "Clear.h"
#include "Message.h"
#include "Sound.h"
#include "Script.h"
#include "Pilon.h"
#include "Arrow.h"
#include "Pause.h"
#include "Push.h"
#include "Select.h"
#include "Outcome.h"
#include "Answer.h"
#include "VariableValue.h"
#include "Random.h"
#include "Danger.h"

namespace ScenariosEditorScenarioElement
{
	static std::vector<std::shared_ptr<ScenarioElement>> ScenarioElements;

	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddElement(std::vector<std::string>* args)
	{
		if (Uzel::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Uzel>(args));
		else if (Start::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Start>(args));
		else if (Clear::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Clear>(args));
		else if (Message::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Message>(args));
		else if (Sound::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Sound>(args));
		else if (Script::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Script>(args));
		else if (Pilon::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Pilon>(args));
		else if (Arrow::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Arrow>(args));
		else if (Pause::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Pause>(args));
		else if (Push::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Push>(args));
		else if (Select::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Select>(args));
		else if (Outcome::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Outcome>(args));
		else if (Answer::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Answer>(args));
		else if (VariableValue::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<VariableValue>(args));
		else if (Random::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Random>(args));
		else if (Danger::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_shared<Danger>(args));
		return ScenarioElements[ScenarioElements.size() - 1];
	}
}