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
}