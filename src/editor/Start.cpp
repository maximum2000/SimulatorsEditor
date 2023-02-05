#include "Start.h"
namespace ScenariosEditorScenarioElement
{
	const std::string Start::ElementName = "Start";
	Start::Start(std::vector<std::string>* args) : ScenarioElement::ScenarioElement(args)
	{
		Name = SetAttributeFromSource(args, 0);
		Description = SetAttributeFromSource(args, 0);
		start_user_x = SetAttributeFromSource(args, 2);
		start_user_y = SetAttributeFromSource(args, 2);
		start_user_z = SetAttributeFromSource(args, 2);
		start_user_angle = SetAttributeFromSource(args, 2);
		start_scene = SetAttributeFromSource(args, 0);
		start_state_file = SetAttributeFromSource(args, 0);
	}
	Start::Start() : ScenarioElement::ScenarioElement()
	{
		Name = ElementAttribute(u8"Name",0);
		Description = ElementAttribute(u8"Description",0);
		start_user_x = ElementAttribute(u8"start_user_x",2);
		start_user_y = ElementAttribute(u8"start_user_y",2);
		start_user_z = ElementAttribute(u8"start_user_z",2);
		start_user_angle = ElementAttribute(u8"start_user_angle",2);
		start_scene = ElementAttribute(u8"start_scene",0);
		start_state_file = ElementAttribute(u8"start_state_file",0);
	}
	std::vector<ElementAttribute*> Start::GetAttributes()
	{
		std::vector<ElementAttribute*> ret;
		ret.emplace_back(&Name);
		ret.emplace_back(&Description);
		ret.emplace_back(&start_user_x);
		ret.emplace_back(&start_user_y);
		ret.emplace_back(&start_user_z);
		ret.emplace_back(&start_user_angle);
		ret.emplace_back(&start_scene);
		ret.emplace_back(&start_state_file);
		return ret;
	}
}