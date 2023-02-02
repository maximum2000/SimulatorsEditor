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