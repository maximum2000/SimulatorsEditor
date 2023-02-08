#include "CanvasPositioning.h"

namespace ScenariosEditorCanvasPositioning
{

	/**x *= 230.0f;
	*y *= -200.0f;*/
	ImVec2* CanvasScrollingRef;
	static ImVec2 Offset(2.5 * 230.0f, -4.75 * -200.0f);
	static ImVec2 CurrentScenarioCenter;
	
	void SetCanvasScrollingRef(ImVec2* scrolling)
	{
		CanvasScrollingRef = scrolling;
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
}