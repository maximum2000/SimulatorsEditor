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
	static float minx = 0, maxy;
	void GetMinimalCoordinate();
	void LoadElements();
	void CoordinatesOldToNew(float *x, float *y);
	void Test()
	{
	}
	pugi::xml_document Model;
	void ScenariosDOM::LoadFrom(const char* path)
	{
		pugi::xml_parse_result result = Model.load_file(path, pugi::parse_default, pugi::encoding_utf8);
		std::cout << "Error description: " << result.description() << "\n";													// debug
		std::cout << "Error offset: " << result.offset << " (error at [..." << (path + result.offset) << "]\n\n";
		maxy = std::stof(std::string(Model.child("root").child("scenarions2").child("elements").first_child().child("y").child_value()));
		GetMinimalCoordinate();
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
		// 10 8
		// 2720 1975
		pugi::xml_node ElementParentNode = Model.child("root").child("scenarions2").child("elements");
		for (pugi::xml_node Element : ElementParentNode.children())
		{
			std::string name = Element.child("etalon").child("name").child_value();
			std::transform(name.begin(), name.end(), name.begin(),
				[](unsigned char c) { return std::tolower(c); });
			float x = std::stof(std::string(Element.child("x").child_value())) - minx;
			float y = (maxy - std::stof(std::string(Element.child("y").child_value())));
			CoordinatesOldToNew(&x, &y);
			ScenariosEditorGUI::AddElement(name.c_str(), x + 10, y + 10);
		}
	}
	void GetMinimalCoordinate()
	{
		pugi::xml_node ElementParentNode = Model.child("root").child("scenarions2").child("elements");
		for (pugi::xml_node Element : ElementParentNode.children())
		{
			if (std::stof(std::string(Element.child("x").child_value())) < minx) minx = std::stof(std::string(Element.child("x").child_value()));
			if (std::stof(std::string(Element.child("y").child_value())) > maxy) maxy = std::stof(std::string(Element.child("y").child_value()));
		}
	}
	void CoordinatesOldToNew(float* x, float* y)
	{
		*x *= 260.0;
	    *y *= 210.0f;
	}
}