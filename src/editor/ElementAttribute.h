#pragma once
#include <string>
#include <vector>

namespace ScenariosEditorScenarioElement
{
	class ElementAttribute
	{
	public:
		ElementAttribute(std::string Name, std::string  ValueF, std::string ValueS, int Format);
		ElementAttribute(std::string Name, int Format);
		ElementAttribute() {}
		std::string Name, ValueS;
		float ValueF = 0;
		int GetFormat();
	protected:
		
		int Format = -1;
		/*
		Format:
		0 - string
		1 - bool
		2 - float
		*/
	};
	
	ElementAttribute SetAttributeFromSource(std::vector<std::string>* args, int Format);
}