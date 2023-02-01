#include "ScenarioElementsStorage.h"
#include <vector>
#include "ScenarioElement.h"
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
	static std::vector<std::unique_ptr<ScenarioElement>> ScenarioElements;

	void AddElement(std::vector<std::string>* args)
	{
		if (Uzel::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Uzel>(args));
		else if (Start::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Start>(args));
		else if (Clear::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Clear>(args));
		else if (Message::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Message>(args));
		else if (Sound::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Sound>(args));
		else if (Script::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Script>(args));
		else if (Pilon::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Pilon>(args));
		else if (Arrow::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Arrow>(args));
		else if (Pause::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Pause>(args));
		else if (Push::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Push>(args));
		else if (Select::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Select>(args));
		else if (Outcome::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Outcome>(args));
		else if (Answer::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Answer>(args));
		else if (VariableValue::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<VariableValue>(args));
		else if (Random::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Random>(args));
		else if (Danger::ElementName == (*args)[0]) ScenarioElements.emplace_back(std::make_unique<Danger>(args));
	}
}