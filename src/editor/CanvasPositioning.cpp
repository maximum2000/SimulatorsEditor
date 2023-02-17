#include "CanvasPositioning.h"
#include <iostream>
#include "scenariosgui.h"

namespace ScenariosEditorCanvasPositioning
{

	/**x *= 230.0f;
	*y *= -200.0f;*/
	ImVec2* CanvasScrollingRef;
	float* CanvasZoomRef;
	static ImVec2 Offset(2.5 * 230.0f, -4.75 * -200.0f);
	static ImVec2 CurrentScenarioCenter;
	static ImVec2 CurrentSize;

	void SetCanvasScrollingRef(ImVec2* scrolling)
	{
		CanvasScrollingRef = scrolling;
	}
	void SetCanvasZoomRef(float* Zoom)
	{
		CanvasZoomRef = Zoom;
	}
	void SetScreenPos(float x, float y)
	{
		*CanvasScrollingRef = ImVec2(-x + Offset.x, -y + Offset.y);
	}
	void SetDefaulScreenPos()
	{
		*CanvasScrollingRef = Offset;
	}
	void SetCurrentScenarioCenter(float x, float y)
	{
		CurrentScenarioCenter = ImVec2(x, y);
	}
	void CenterScenario()
	{
		SetScreenPos(CurrentScenarioCenter.x, CurrentScenarioCenter.y);
	}
	void SetSize(ImVec2 Size)
	{
		CurrentSize = Size;
	}
	void CenterView(int x, int y)
	{
		*CanvasScrollingRef = ImVec2(-x + CurrentSize.x/2, -y + CurrentSize.y/2);
	}
}