#include "Sound.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Sound::ElementName = "sound";
	Sound::Sound(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Name = SetAttributeFromSource(args, 0);
	}
	Sound::Sound() : ScenarioElement::ScenarioElement()
	{
		Name = ElementAttribute(u8"Name",0);
	}
	std::vector<ElementAttribute*> Sound::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Name);
		return ret;
	}
}