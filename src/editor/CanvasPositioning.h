#pragma once
#include "imgui.h"

namespace ScenariosEditorCanvasPositioning
{
	void SetCanvasScrollingRef(ImVec2* scrolling);
	void SetDefaulScreenPos();
	void CenterScenario();
	void SetScreenPos(float x, float y);
	void SetCurrentScenarioCenter(float x, float y);
}