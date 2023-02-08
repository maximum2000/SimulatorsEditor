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
	static int MaxPin = 0;
	std::vector<std::vector<int>> Links;
	static std::vector<std::shared_ptr<ScenarioElement>> ScenarioElements;
	void SetMinimalCoordinates();
	void SetMinimalCoordinates(std::vector<std::shared_ptr<ScenarioElement>> Optimized);
	void CoordinatesOldToNew(float* x, float* y);
	void UpdateCoordinates(float* x, float* y);
	int GetPoint(const char* name, int pin_index);
	// Add new element
	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddScenarioElementStorageElement(std::string ElementName, float x, float y, float alfa, int pin_count)
	{
		if (Uzel::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Uzel>());
		else if (Start::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Start>());
		else if (Clear::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Clear>());
		else if (Message::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Message>());
		else if (Sound::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Sound>());
		else if (Script::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Script>());
		else if (Pilon::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Pilon>());
		else if (Arrow::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Arrow>());
		else if (Pause::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Pause>());
		else if (Push::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Push>());
		else if (Select::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Select>());
		else if (Outcome::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Outcome>());
		else if (Answer::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Answer>());
		else if (VariableValue::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<VariableValue>());
		else if (Random::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Random>());
		else if (Danger::ElementName == ElementName) ScenarioElements.emplace_back(std::make_shared<Danger>());
		(*ScenarioElements[ScenarioElements.size() - 1]).ScenarioGUID = ScenarioEditorScenarioStorage::GetActualGUID();
		x /= 230.0f;
		y /= 200.0f;
		x += minx;
		y = maxy - y;
		(*ScenarioElements[ScenarioElements.size() - 1]).x = x;
		(*ScenarioElements[ScenarioElements.size() - 1]).y = y;
		(*ScenarioElements[ScenarioElements.size() - 1]).alfa = alfa;
		for (int i = 0; i < pin_count; i++)
			(*ScenarioElements[ScenarioElements.size() - 1]).pins.push_back(++MaxPin);
		return ScenarioElements[ScenarioElements.size() - 1];
	
	}
	// Copy element
	std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> AddScenarioElementStorageElement(std::string ElementName, float x, float y, float alfa, int pin_count, std::shared_ptr<ScenarioElement> CopyOrigin)
	{
		std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> ret = AddScenarioElementStorageElement(ElementName, x, y, alfa, pin_count);
		(*ret).caption = (*CopyOrigin).caption;
		for (int i = 0; i < (*CopyOrigin).GetAttributes().size(); i++)
		{
			(*ret).GetAttributes()[i]->Name = (*CopyOrigin).GetAttributes()[i]->Name;
			(*ret).GetAttributes()[i]->ValueF = (*CopyOrigin).GetAttributes()[i]->ValueF;
			(*ret).GetAttributes()[i]->ValueS = (*CopyOrigin).GetAttributes()[i]->ValueS;
		}
		return ret;

	}
	// Add existing element
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
		for (int Pin : (*ScenarioElements[ScenarioElements.size() - 1]).pins)
		{
			if (Pin > MaxPin) MaxPin = Pin;
		}
		return ScenarioElements[ScenarioElements.size() - 1];
	}

	void AddScenarioElementStorageLink(std::vector<int> args)
	{
		Links.push_back(args);
	}

	void RemoveScenarioElementStorageElement(std::shared_ptr<ScenarioElement> Element)
	{
		MaxPin = 0;
		for (int i = 0; i < ScenarioElements.size(); i++)
		{
			if (Element == ScenarioElements[i])
			{
				ScenarioElements.erase(ScenarioElements.begin() + i);
				return;
			}
			for (int Pin : (*ScenarioElements[i]).pins)
			{
				if (Pin > MaxPin) MaxPin = Pin;
			}
		}
		
	}

	void RemoveScenarioElementStorageLink(std::vector<int> args)
	{
		for (int i = 0; i < Links.size(); i++)
		{
			if (args[0] == Links[i][0])
			{
				if (args[1] == Links[i][1])
				{
					Links.erase(Links.begin() + i);
					return;
				}
			}
		}
	}

	void ClearScenarioElementStorage()
	{
		ScenarioElements.clear();
		Links.clear();
	}

	void LoadElements()
	{
		ScenariosEditorGUI::ClearElements();
		std::string ActualGUID = ScenarioEditorScenarioStorage::GetActualGUID();
		std::vector<std::shared_ptr<ScenarioElement>> OptimizedVector;
		for (std::shared_ptr<ScenarioElement> Element : ScenarioElements)
		{
			if ((*Element).ScenarioGUID != ActualGUID) continue;
			OptimizedVector.push_back(Element);
		}
		SetMinimalCoordinates(OptimizedVector);
		
		for (std::shared_ptr<ScenarioElement> Element : OptimizedVector)
		{
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
			for (std::shared_ptr<ScenarioElement> Element : OptimizedVector)
			{
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

	void SetMinimalCoordinates(std::vector<std::shared_ptr<ScenarioElement>> Optimized)
	{
		minx = std::numeric_limits<float>::max();
		maxy = 0;
		for (std::shared_ptr<ScenarioElement> Element : Optimized)
		{
			if ((*Element).x < minx) minx = (*Element).x;
			if ((*Element).y > maxy) maxy = (*Element).y;
		}
		if (minx == std::numeric_limits<float>::max()) minx = 0;
	}

	void SetMinimalCoordinates()
	{
		minx = std::numeric_limits<float>::max();
		maxy = 0;
		for (std::shared_ptr<ScenarioElement> Element : ScenarioElements)
		{
			if ((*Element).ScenarioGUID != ScenarioEditorScenarioStorage::GetActualGUID()) continue;
			if ((*Element).x < minx) minx = (*Element).x;
			if ((*Element).y > maxy) maxy = (*Element).y;
		}
		if (minx == std::numeric_limits<float>::max()) minx = 0;
	}

	void CoordinatesOldToNew(float* x, float* y)
	{
		*x *= 230.0f;
		*y *= 200.0f;
	}

	void UpdateCoordinates(std::shared_ptr<ScenarioElement> Elem, float new_x, float new_y)
	{
		float x = new_x;
		float y = new_y;
		x /= 230.0f;
		y /= 200.0f;
		x += minx;
		y = maxy - y;
		(*Elem).x = x;
		(*Elem).y = y;
	}

	int GetPoint(const char* name, int pin_index)
	{

		int ret = pin_index == 0 ? 2 : 0;
		if (!strcmp("uzel", name)) return 0;
		if (!strcmp("Start", name)) return 2;
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

	// reverse to getpoint
	int GetPinIndex(const char* name, int pin)
	{
		int ret = pin == 2 ? 0 : 1;
		if (!strcmp("uzel", name)) return 0;
		if (!strcmp("answer", name) || !strcmp("variable_value", name) || !strcmp("random", name) || !strcmp("danger", name))
		{
			switch (pin)
			{
			case 0: return 0;
			case 1: return 2;
			case 3: return 1;
			}
		}
		return ret;
	}

	std::vector<std::shared_ptr<ScenarioElement>> GetScenarioElements()
	{
		return ScenarioElements;
	}

	std::vector<std::vector<int>> GetLinks()
	{
		return Links;
	}

	void SetMaxPin(int max_pin)
	{
		MaxPin = max_pin;
	}
}