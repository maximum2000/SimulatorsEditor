#pragma once
#include "ScenarioElement.h"
#include "ElementAttribute.h"

namespace ScenariosEditorScenarioElement
{
	class Start : public ScenarioElement
	{
	public:
		const static std::string ElementName;
		Start(std::vector<std::string>* args);
		std::vector<ElementAttribute*> GetAttributes();
	private:
		ElementAttribute Name, Description, start_user_x, start_user_y, start_user_z, start_user_angle, start_scene, start_state_file;
	};
}