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

#pragma execution_character_set("utf-8")

namespace ScenariosEditorXML
{
	static float minx = 0, maxy;
	static int InsertAt;
	static std::vector<std::vector<int>> Pins;
	void GetMinimalCoordinate();
	void LoadElements();
	void CoordinatesOldToNew(float *x, float *y);
	int GetPoint(const char* name, int pin_index);
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
			std::vector<int> ElementPins;
			pugi::xml_node CurrentPin = Element.child("pins").first_child();
			while (CurrentPin != nullptr)
			{
				ElementPins.push_back(std::stof(std::string(CurrentPin.child("id").child_value())));
				CurrentPin = CurrentPin.next_sibling();
			}
			Pins.push_back(ElementPins);
		}
		pugi::xml_node LinksParentNode = Model.child("root").child("scenarions2").child("scenarion_links");
		for (pugi::xml_node Link : LinksParentNode.children())
		{
			int ElemA = -1, ElemB = -1;
			int PointA = -1, PointB = -1;
			int PinA = std::stof(std::string(Link.child("pinA").child_value()));
			int PinB = std::stof(std::string(Link.child("pinB").child_value()));
			for (int i = 0; i < Pins.size(); i++)
			{
				int count = 2;
				std::vector<int> CurrentElemPins = Pins[i];
				for (int j = 0; j < CurrentElemPins.size(); j++)
				{
					if (CurrentElemPins[j] == PinA)
					{
						count--;
						ElemA = i;
						const char* test = ScenariosEditorGUI::GetNameOfElementOnCanvas(ElemA);
						PointA = GetPoint(ScenariosEditorGUI::GetNameOfElementOnCanvas(ElemA), j);
					}
					if (CurrentElemPins[j] == PinB)
					{
						count--;
						ElemB = i;
						PointB = GetPoint(ScenariosEditorGUI::GetNameOfElementOnCanvas(i), j);
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
		pugi::xml_node ElementParentNode = Model.child("root").child("scenarions2").child("elements");
		for (pugi::xml_node Element : ElementParentNode.children())
		{
			if (std::stof(std::string(Element.child("x").child_value())) < minx) minx = std::stof(std::string(Element.child("x").child_value()));
			if (std::stof(std::string(Element.child("y").child_value())) > maxy) maxy = std::stof(std::string(Element.child("y").child_value()));
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