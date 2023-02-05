#include "ElementAttribute.h"
namespace ScenariosEditorScenarioElement
{
	ElementAttribute::ElementAttribute(std::string name, std::string ValueF, std::string ValueS, int Format)
	{
		this->Name = name;
		this->ValueF = std::stof(ValueF);
		this->ValueS = ValueS;
		this->Format = Format;
	}
	ElementAttribute SetAttributeFromSource(std::vector<std::string>* args, int Format)
	{
		ElementAttribute ret = ElementAttribute((*args)[0], (*args)[1], (*args)[2], Format);
		(*args).erase((*args).begin(), (*args).begin() + 3);
		return ret;
	}
	ElementAttribute::ElementAttribute(std::string Name, int Format) {
		this->Name = Name;
		ValueF = 0.00f;
		ValueS = "";
		this->Format = Format;
	}
	int ElementAttribute::GetFormat()
	{
		return Format;
	}
}