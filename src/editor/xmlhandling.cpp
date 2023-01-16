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

#pragma execution_character_set("utf-8")

namespace ScenariosEditorXML
{
	void LoadElements();
	void Test()
	{
	}
	pugi::xml_document Model;
	void ScenariosDOM::LoadFrom(const char* path)
	{
		pugi::xml_parse_result result = Model.load_file(path, pugi::parse_default, pugi::encoding_utf8);
		std::cout << "Error description: " << result.description() << "\n";													// debug
		std::cout << "Error offset: " << result.offset << " (error at [..." << (path + result.offset) << "]\n\n";
		LoadElements();
	}
	void ScenariosDOM::SaveTo(const char* path)
	{
		std::cout << Model.save_file(path);
	}
	void LoadElements()
	{
		// root
		//		scenarions2
		//			elements
		//				element  <-  !!!
		//				element  <-  !!!
		pugi::xml_node ElementParentNode = Model.child("root").child("scenarions2").child("elements");
		for (pugi::xml_node Element : ElementParentNode.children())
		{
			std::string name = Element.child("etalon").child("name").child_value();
			std::transform(name.begin(), name.end(), name.begin(),
				[](unsigned char c) { return std::tolower(c); });
			ScenariosEditorGUI::AddElement(name.c_str(),
				std::stof(std::string(Element.child("x").child_value())),
				std::stof(std::string(Element.child("y").child_value())));
			std::cout << std::stof(std::string(Element.child("x").child_value()));
		}
	}
}