#include "Sound.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Sound::ElementName = "sound";
	Sound::Sound(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Name = SetAttributeFromSource(args, 0);
	}
}