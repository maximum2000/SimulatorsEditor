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
#include "scenariosgui.h"

namespace ScenariosEditorScenarioElement
{
	static float minx, maxy;
	std::vector<std::vector<int>> Links;
	static std::vector<std::shared_ptr<ScenarioElement>> ScenarioElements;
	void GetMinimalCoordinate();
	void CoordinatesOldToNew(float* x, float* y);
	int GetPoint(const char* name, int pin_index);

	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddScenarioElementStorageElement(std::vector<std::string>* args)
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
	void AddScenarioElementStorageLink(std::vector<int> args)
	{
		Links.push_back(args);
	}
	void ClearScenarioElementStorage()
	{
		ScenarioElements.clear();
	}
	void LoadElements()
	{
		ScenariosEditorGUI::ClearElements();
		std::string ActualGUID = ScenarioEditorScenarioStorage::GetActualGUID();
		minx = std::numeric_limits<float>::max();
		maxy = std::numeric_limits<float>::min();
		GetMinimalCoordinate();
		for (std::shared_ptr<ScenarioElement> Element : ScenarioElements)
		{
			if ((*Element).ScenarioGUID != ActualGUID) continue;
			float x = (*Element).x - minx;
			float y = maxy - (*Element).y;
			CoordinatesOldToNew(&x, &y);
			ScenariosEditorGUI::AddElement((*Element).getElementName().c_str(), x + 10, y + 10, Element);
		}
		for (std::vector<int> Link : Links)
		{
			int ElemA = -1, ElemB = -1;
			int PointA = -1, PointB = -1;
			int PinA = Link[0];
			int PinB = Link[1];
			for (std::shared_ptr<ScenarioElement> Element : ScenarioElements)
			{
				if ((*Element).ScenarioGUID != ActualGUID) continue;
				int count = 2;
				std::vector<int> CurrentElemPins = (*Element).pins;
				for (int j = 0; j < CurrentElemPins.size(); j++)
				{
					if (CurrentElemPins[j] == PinA)
					{
						count--;
						ElemA = ScenariosEditorGUI::GetNumOfElement(Element);
						PointA = GetPoint(ScenariosEditorGUI::GetNameOfElementOnCanvas(ElemA), j);
					}
					if (CurrentElemPins[j] == PinB)
					{
						count--;
						ElemB = ScenariosEditorGUI::GetNumOfElement(Element);
						PointB = GetPoint(ScenariosEditorGUI::GetNameOfElementOnCanvas(ElemB), j);
					}
					if (!count) break;
				}
			}
			if (ElemA != -1 && ElemB != -1)
			{
				ScenariosEditorGUI::AddLink(ElemA, ElemB, PointA, PointB);
			}
		}
	}
	void GetMinimalCoordinate()
	{
		for (std::shared_ptr<ScenarioElement> Element : ScenarioElements)
		{
			if ((*Element).ScenarioGUID != ScenarioEditorScenarioStorage::GetActualGUID()) continue;
			if ((*Element).x < minx) minx = (*Element).x;
			if ((*Element).y > maxy) maxy = (*Element).y;
		}
	}
	void CoordinatesOldToNew(float* x, float* y)
	{
		*x *= 230.0f;
		*y *= 200.0f;
	}
	int GetPoint(const char* name, int pin_index)
	{
		int ret = pin_index == 0 ? 2 : 0;
		if (!strcmp("uzel", name)) return 0;
		if (!strcmp("start", name)) return 2;
		if (!strcmp("answer", name) || !strcmp("variable_value", name) || !strcmp("random", name) || !strcmp("danger", name))
		{
			switch (pin_index)
			{
			case 0: return 0;
			case 1: return 3;
			case 2: return 1;
			}
		}
		return ret;
	}
}