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
#include <iomanip>
#include <sstream>
#pragma comment(lib, "Rpcrt4.lib")

#pragma execution_character_set("utf-8")

namespace ScenariosEditorXML
{
	void LoadElementsToStorage();
	void LoadScenariosToStorage();
	void LoadElementsFromStorage();
	void InitializeDOM();
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
		LoadElementsFromStorage();
		Model.save_file(path);
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
	// fix later
	void LoadElementsFromStorage()
	{
		if (Model.child("root") == nullptr) InitializeDOM();
		Model.child("root").child("scenarions2").remove_children();
		for (std::vector<std::string> Scenario : ScenarioEditorScenarioStorage::GetScenarios())
		{
			if (Scenario[1] == "") continue;
			pugi::xml_node ScenarioNode = Model.child("root").child("scenarions2").append_child("scenario");
			ScenarioNode.append_child("startGUID");
			ScenarioNode.child("startGUID").text().set(Scenario[1].c_str());
			ScenarioNode.append_child("Name");
			ScenarioNode.child("Name").text().set(Scenario[0].c_str());
		}
		Model.child("root").child("scenarions2").append_child("elements");
		for (std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> Element : ScenariosEditorScenarioElement::GetScenarioElements())
		{
			pugi::xml_node ElementNode = Model.child("root").child("scenarions2").child("elements").append_child("element");
			ElementNode.append_child("ScenarioGUID");
			ElementNode.child("ScenarioGUID").text().set((*Element).ScenarioGUID.c_str());
			ElementNode.append_child("x");
			ElementNode.child("x").text().set(std::to_string((*Element).x).c_str());
			ElementNode.append_child("y");
			ElementNode.child("y").text().set(std::to_string((*Element).y).c_str());
			ElementNode.append_child("alfa");
			std::stringstream Float;
			Float << std::fixed << std::setprecision(2) << (*Element).alfa;
			ElementNode.child("alfa").text().set(Float.str().c_str());
			ElementNode.append_child("pins");
			for (int Pin : (*Element).pins)
			{
				ElementNode.child("pins").append_child("pin").append_child("id").text().set(Pin);
			}
			pugi::xml_node Etalon = ElementNode.append_child("etalon");
			Etalon.append_child("name").text().set((*Element).getElementName().c_str());
			Etalon.append_child("caption").text().set((*Element).caption.c_str());
			Etalon.append_child("attributes");
			for (ScenariosEditorScenarioElement::ElementAttribute* Attribute : (*Element).GetAttributes())
			{ 
				pugi::xml_node Node = Etalon.child("attributes").append_child("scenario_attribute");
				Node.append_child("name").text().set(Attribute->Name.c_str());
				std::stringstream Float;
				Float << std::fixed << std::setprecision(2) << Attribute->ValueF;
				Node.append_child("valueF").text().set(Float.str().c_str());
				Node.append_child("valueS").text().set(Attribute->ValueS.c_str());
			}
		}
		pugi::xml_node Links = Model.child("root").child("scenarions2").append_child("scenarion_links");
		for (std::vector<int> Link : ScenariosEditorScenarioElement::GetLinks())
		{
			pugi::xml_node LinkNode = Links.append_child("scenarion_link");
			LinkNode.append_child("pinA").text().set(Link[0]);
			LinkNode.append_child("pinB").text().set(Link[1]);
		}
	}

	void InitializeDOM()
	{
		pugi::xml_node root = Model.append_child("root");
		pugi::xml_node screen = root.append_child("screen");
		screen.append_child("zoom").text().set("5.599999");
		screen.append_child("x").text().set("0");
		screen.append_child("y").text().set("0");
		root.append_child("elements");
		root.append_child("links");
		root.append_child("scenarions2");
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