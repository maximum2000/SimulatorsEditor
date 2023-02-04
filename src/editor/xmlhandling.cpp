//
// XML-Canvas connection
//

#include "xmlhandling.h"
#include <iostream>
#include "pugixml.hpp"
#include "scenariosgui.h"
#include <algorithm>
#include <cctype>
#include <string>
#include <vector>
#include "ScenarioElementsStorage.h"
#include "ScenarioStorage.h"
#include <rpc.h>
#include <rpcdce.h>
#pragma comment(lib, "Rpcrt4.lib")

#pragma execution_character_set("utf-8")

namespace ScenariosEditorXML
{
	void LoadElementsToStorage();
	void LoadScenariosToStorage();
	std::vector<std::string> GetArguments(pugi::xml_node Element);
	pugi::xml_document Model, Testing;
	void ScenariosDOM::LoadFrom(const wchar_t* path)
	{
		pugi::xml_parse_result result = Model.load_file(path, pugi::parse_default, pugi::encoding_utf8);
		LoadElementsToStorage();
		LoadScenariosToStorage();
	}
	bool ScenariosDOM::CheckFile(const wchar_t* path)
	{
		if (Testing.load_file(path, pugi::parse_default, pugi::encoding_utf8).status != pugi::status_ok) return false;
		else return true;
	}
	void ScenariosDOM::SaveTo(const wchar_t* path)
	{
		pugi::xml_parse_result result = Model.load_file(path, pugi::parse_default, pugi::encoding_utf8);
		std::cout << Model.save_file(path);
	}
	void LoadElementsToStorage()
	{
		pugi::xml_node ElementParentNode = Model.child("root").child("scenarions2").child("elements");
		for (pugi::xml_node Element : ElementParentNode.children())
		{
			std::vector<std::string> args = GetArguments(Element);
			ScenariosEditorScenarioElement::AddScenarioElementStorageElement(&args);
			std::string name = Element.child("etalon").child("name").child_value();
		}
		pugi::xml_node LinksParentNode = Model.child("root").child("scenarions2").child("scenarion_links");
		for (pugi::xml_node Link : LinksParentNode.children())
		{
			int PinA = std::stof(std::string(Link.child("pinA").child_value()));
			int PinB = std::stof(std::string(Link.child("pinB").child_value()));
			std::vector<int> ToAdd;
			ToAdd.push_back(PinA);
			ToAdd.push_back(PinB);
			ScenariosEditorScenarioElement::AddScenarioElementStorageLink(ToAdd);
		}
	}
	void LoadScenariosToStorage()
	{
		ScenarioEditorScenarioStorage::ClearScenarioStorage();
		pugi::xml_node ScenarioParentNode = Model.child("root").child("scenarions2");
		for (pugi::xml_node Scenario : ScenarioParentNode.children())
		{
			if (std::strcmp(Scenario.name(), "scenario"))
			{
				return;
			}
			const char * charguid = Scenario.child("startGUID").child_value();
			unsigned char ucharguid[50];
			int Current = 0;
			while (charguid[Current] != '\0')
			{
				std::cout << charguid[Current] << '\n';
				ucharguid[Current] = (unsigned char)charguid[Current];
				Current++;
			}
			ucharguid[Current] = '\0';
			UUID ToAdd;
			UuidFromStringA(ucharguid, &ToAdd);
			std::string ScenarioName = Scenario.child("Name").child_value();
			if (Scenario.child("Name").child_value() == "") ScenarioName = u8"<Без имени>";
			ScenarioEditorScenarioStorage::AddScenarioStorageElement(
				ScenarioName,
				ToAdd
			);
		}
	}
	std::vector<std::string> GetArguments(pugi::xml_node Element)
	{
		// same order as xml, with exception: name of element placed first
		std::vector<std::string> args;
		args.push_back(std::string(Element.child("etalon").child("name").child_value()));
		args.push_back(std::string(Element.child("ScenarioGUID").child_value()));
		args.push_back(std::string(Element.child("x").child_value()));
		args.push_back(std::string(Element.child("y").child_value()));
		args.push_back(std::string(Element.child("alfa").child_value()));
		pugi::xml_node CurrentPin = Element.child("pins").first_child();
		while (CurrentPin != nullptr)
		{
			args.push_back(std::string(CurrentPin.child("id").child_value()));
			CurrentPin = CurrentPin.next_sibling();
		}
		args.push_back("Separator_Pins");
		args.push_back(std::string(Element.child("etalon").child("caption").child_value()));
		pugi::xml_node CurrentAttribute = Element.child("etalon").child("attributes").first_child();
		while (CurrentAttribute != nullptr)
		{
			args.push_back(std::string(CurrentAttribute.child("name").child_value()));
			args.push_back(std::string(CurrentAttribute.child("valueF").child_value()));
			args.push_back(std::string(CurrentAttribute.child("valueS").child_value()));
			CurrentAttribute = CurrentAttribute.next_sibling();
		}
		return args;
	}
}