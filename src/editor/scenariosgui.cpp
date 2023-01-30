//
// ImGui-based UI functions
//

/*

Known problems:

-Loading elements should clear Elems
-Highlighting (static struct of colors, link)

*/

#include <tchar.h>
#include <iostream>
#include <vector>
#include <d3d11.h>
#include "imgui.h"

#include "xmlhandling.h"
#include "render.h"
#include "ElementsData.h"

using namespace ScenariosEditorElementsData;

namespace ScenariosEditorGUI {

	enum CanvasState
	{
		Rest,
		Selection,
		ElementDragging,
		CanvasDragging
	};

	static CanvasState CurrentState = Rest;

	// Xml data
	static ScenariosEditorXML::ScenariosDOM Model;

	// ImGui data
	const ImGuiViewport* viewport;
	ImDrawList* draw_list;

	// Helper data
	struct ElementOnCanvas
	{
		int Element;
		ImVec2 Pos;
		int Type;
	};

	struct LinkOnCanvas
	{
		int Points[2];
		int Elems[2];
	};

	static std::vector<ElementOnCanvas> Elems;
	static std::vector<LinkOnCanvas> Links;
	static std::vector<ElementOnCanvas> CopyBuffer;
	static std::vector<int> CopyBufferID;
	static std::vector<int> SelectedElems;
	static ImVec2 MousePos, origin;
	static bool IsLinking = false;


	void PreLoopSetup();
	void MainLoop();

	void DrawMenu();
	void DrawCanvas();
	void DrawScenariosSection();
	void DrawElementsSection();
	void DrawParamsSection();
	void CanvasLogic(int ClickedOn, ImVec2* SelectionStartPosition);

	// Helper functions
	void ElementsDrawObjects();
	void AddCanvasContextMenu();
	void AddDragAndDropReciever();
	void CanvasDrawLinking();
	void CanvasDrawElems(ImGuiIO& io, int* ClickedOn, ImVec2* SelectionStartPosition);
	void ParamsInitialization();


	// Should be hooked from main
	void ShowDemoScenarioGUI()
	{
		ScenariosEditorRender::InitializeWindowAndImGUIContext();
		PreLoopSetup();
		MainLoop();
		ScenariosEditorRender::EraseWindowAndImGuiContext();
	}

	// Contains instructions should be executed before main loop
	void PreLoopSetup()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsLight();
		io.Fonts->AddFontFromFileTTF(u8"C:/Users/VR/Desktop/projects/SimulatorsEditor/src/editor/LiberationSans.ttf", 22.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
		ElementsData::Initialization();
	}

	// main loop
	void MainLoop()
	{
		bool done = false;
		while (!done)
		{
			std::cout << CurrentState;
			if (ScenariosEditorRender::HandleMessages() || done)
				break;

			ScenariosEditorRender::StartFrame();
			viewport = ImGui::GetMainViewport();

			// Menu
			DrawMenu();
			// Window
			DrawCanvas();
			DrawScenariosSection();
			DrawElementsSection();
			DrawParamsSection();

			ImGui::ShowDemoWindow(); // remove later
			ScenariosEditorRender::EndFrame();
		}
	}

	// menu
	void DrawMenu()
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu(u8"Файл"))
		{
			if (ImGui::MenuItem(u8"Открыть", "Ctrl+O"))
			{
				Model.LoadFrom(u8"C:\\xmltest\\xmlold.model");
			}
			//if (ImGui::MenuItem(u8"Недавние файлы..."))
			//{
			//    //
			//    ImGui::EndMenu();
			//}
			if (ImGui::MenuItem(u8"Сохранить", "Ctrl+S"))
			{
				Model.SaveTo(u8"C:\\xmltest\\xmlpugi.model");
			}
			if (ImGui::MenuItem(u8"Сохранить как...")) {}
			ImGui::EndMenu();

		}
		/* if (ImGui::BeginMenu("Edit"))
			 {
				 if (ImGui::MenuItem("Undo", "CTRL+Z")) {}
				 if (ImGui::MenuItem("Cut", "CTRL+X")) {}
				 if (ImGui::MenuItem("Copy", "CTRL+C")) {}
				 if (ImGui::MenuItem("Paste", "CTRL+V")) {}
				 ImGui::EndMenu();
			 }*/
		ImGui::EndMainMenuBar();
	}

	static ImVec2 scrolling(0.0f, 0.0f); // current scrolling

	// canvas section
	void DrawCanvas()
	{
		ImGuiIO& io = ImGui::GetIO();

		// Canvas size and style
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x / 4, viewport->WorkPos.y));
		ImGui::SetNextWindowSize(ImVec2(3 * viewport->WorkSize.x / 4, viewport->WorkSize.y));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("MainWorkspace", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::PopStyleVar();

		// Canvas positioning
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(240, 240, 240, 0));
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight); // Window itself doesn't trigger io mouse actions, invisible button does
		ImGui::SetItemAllowOverlap();

		// Draw grid
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		{
			const float GRID_STEP = 79.0f;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 119));
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 119));
		}
		draw_list->PopClipRect();

		// canvas data
		int ClickedOn = ImGui::IsItemHovered(); // used for logic
		origin = ImVec2(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y);

		// Add context menu
		AddCanvasContextMenu();

		// Canvas is drag'n'drop reciever
		AddDragAndDropReciever();
		
		static ImVec2 SelectionStartPosition; // used for logic
		CanvasDrawElems(io, &ClickedOn, &SelectionStartPosition);
		CanvasDrawLinking();
		CanvasLogic(ClickedOn, &SelectionStartPosition);
		ImGui::End();
	}

	void AddDragAndDropReciever()
	{
		ImGuiIO& io = ImGui::GetIO();
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
		// canvas is drag'n'drop reciever
		if (ImGui::BeginDragDropTarget()) {

			auto payload = ImGui::AcceptDragDropPayload("Element");
			if (payload != NULL) {
				int ElementNum = *(int*)payload->Data;
				int x = mouse_pos_in_canvas.x - ElementsData::GetElementTexture(ElementNum).Width / 2;
				int y = mouse_pos_in_canvas.y - ElementsData::GetElementTexture(ElementNum).Height / 2;
				if (x < 0) x = 0;
				if (y < 0) y = 0;
				Elems.push_back({ ElementNum, ImVec2(x, y), ElementsData::GetElementType(ElementNum) });
			}
			ImGui::EndDragDropTarget();
		}
	}

	// scenarios section
	void DrawScenariosSection()
	{
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, (viewport->WorkSize.y / 3) - 29));
		ImGui::Begin(u8"Сценарии", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::End();
	}

	// elements section
	void DrawElementsSection()
	{
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, (viewport->WorkSize.y / 3) - 2));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, viewport->WorkSize.y / 3));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 15));
		ImGui::Begin(u8"Элементы сценария", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ImGui::PopStyleVar();
		ElementsDrawObjects();
		ImGui::End();
	}

	// params section
	void DrawParamsSection()
	{
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, (2 * viewport->WorkSize.y / 3) - 4));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, (viewport->WorkSize.y / 3) + 33));
		ImGui::Begin(u8"Свойства", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
		ParamsInitialization();
		ImGui::End();
	}

	// give linking points coordinates depending on elem's type and texture
	ImVec2 GetLinkingPointLocation(int Elem, int Point)
	{
		switch (Point)
		{
		case 0: return ImVec2(origin.x + Elems[Elem].Pos.x + ElementsData::GetElementTexture(Elems[Elem].Element).Width / 2, origin.y + Elems[Elem].Pos.y); break;
		case 1: return ImVec2(origin.x + Elems[Elem].Pos.x + ElementsData::GetElementTexture(Elems[Elem].Element).Width, origin.y + Elems[Elem].Pos.y + ElementsData::GetElementTexture(Elems[Elem].Element).Height / 2); break;
		case 2: return ImVec2(origin.x + Elems[Elem].Pos.x + ElementsData::GetElementTexture(Elems[Elem].Element).Width / 2, origin.y + Elems[Elem].Pos.y + ElementsData::GetElementTexture(Elems[Elem].Element).Height); break;
		case 3: return ImVec2(origin.x + Elems[Elem].Pos.x, origin.y + Elems[Elem].Pos.y + ElementsData::GetElementTexture(Elems[Elem].Element).Height / 2); break;
		}
	}
	
	// add buttons which represent linking points
	void AddLinkingPoint(int Point, int* ClickedElem, int* ClickedType, int Elem)
	{
		draw_list->AddCircleFilled(GetLinkingPointLocation(Elem, Point), 5.0f, IM_COL32(0, 0, 0, 255));
		ImGui::SetCursorScreenPos(ImVec2(GetLinkingPointLocation(Elem, Point).x - 5.0f, GetLinkingPointLocation(Elem, Point).y - 5.0f));
		ImGui::InvisibleButton("LinkingPoint", ImVec2(10.0f, 10.0f), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		ImGui::SetItemAllowOverlap();
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			if (!IsLinking)
			{
				*ClickedElem = Elem;
				*ClickedType = Point;
				SelectedElems.clear();
			}
			else
			{
				if (*ClickedType != Point || *ClickedElem != Elem)
				{
					Links.push_back({ {*ClickedType, Point},{*ClickedElem, Elem} });
				}
			}
			IsLinking = !IsLinking;
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{

		}
	}

	// draw links
	void CanvasDrawLinking()
	{
		ImGuiIO& io = ImGui::GetIO();
		static int ClickedElem;
		static int ClickedType;
		for (int i = 0; i < Elems.size(); i++)
		{
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element);
			if (Elems[i].Type & 1)
			{
				AddLinkingPoint(0, &ClickedElem, &ClickedType, i);
			}
			if (Elems[i].Type & 2)
			{
				AddLinkingPoint(1, &ClickedElem, &ClickedType, i);
			}
			if (Elems[i].Type & 4)
			{
				AddLinkingPoint(2, &ClickedElem, &ClickedType, i);
			}
			if (Elems[i].Type & 8)
			{
				AddLinkingPoint(3, &ClickedElem, &ClickedType, i);
			}
			if (IsLinking)
			{
				draw_list->AddLine(GetLinkingPointLocation(ClickedElem, ClickedType), io.MousePos, IM_COL32(0, 0, 0, 255));
			}
			
		}
		for (int j = 0; j < Links.size(); j++)
		{
			draw_list->AddLine(GetLinkingPointLocation(Links[j].Elems[0], Links[j].Points[0]), GetLinkingPointLocation(Links[j].Elems[1], Links[j].Points[1]), IM_COL32(0, 0, 0, 255));
		}
	}

	// draw elems
	void CanvasDrawElems(ImGuiIO& io, int* ClickedOn, ImVec2* SelectionStartPosition)
	{
		bool mouseoncanvas = *ClickedOn;
		static bool IsDragging = false;
		static ImVec2 OldElementPosition;
		bool IsWorkspaceHovered = ImGui::IsItemHovered();
		ElementOnCanvas ToAdd;
		// https://github.com/ocornut/imgui/issues/3909 doesn't work correctly, solved sideways (Last drawn element stays on top)
		if (CurrentState == Selection) SelectedElems.clear();
		for (int i = 0; i < Elems.size(); i++)
		{
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element);
			draw_list->AddImage((void*)UsedTexture.Payload, ImVec2(origin.x + Elems[i].Pos.x, origin.y + Elems[i].Pos.y), ImVec2(origin.x + Elems[i].Pos.x + UsedTexture.Width, origin.y + Elems[i].Pos.y + UsedTexture.Height));
			ImGui::SetCursorScreenPos(ImVec2(origin.x + Elems[i].Pos.x, origin.y + Elems[i].Pos.y));
			ImGui::InvisibleButton("canvas123", ImVec2(UsedTexture.Width, UsedTexture.Height), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			ImGui::SetItemAllowOverlap();
			// IsMouseClicked() + IsItemHovered() usage to find which element user selected
			if (CurrentState == Rest && ImGui::IsItemHovered())
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
					{
						SelectedElems.clear();
						SelectedElems.push_back(i);
					}
					OldElementPosition = Elems[i].Pos;
				}
				IsWorkspaceHovered = false;
				*ClickedOn = 0;
			}
			if (CurrentState == Selection)
				if (Elems[i].Pos.x + origin.x > (io.MousePos.x + SelectionStartPosition->x - max(io.MousePos.x, SelectionStartPosition->x)) && Elems[i].Pos.x + origin.x < (io.MousePos.x + SelectionStartPosition->x - min(io.MousePos.x, SelectionStartPosition->x)))
					if (Elems[i].Pos.y + origin.y > (io.MousePos.y + SelectionStartPosition->y - max(io.MousePos.y, SelectionStartPosition->y)) && Elems[i].Pos.y + origin.y < (io.MousePos.y + SelectionStartPosition->y - min(io.MousePos.y, SelectionStartPosition->y)))
					{
						if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
							SelectedElems.push_back(i);
					}
		}

		if (IsLinking)
		{
			//IsSelecting = false;
			SelectedElems.clear();
		}
		// dragging, clicking on elements realization
		int j = SelectedElems.size();
		for (int i = 0; i < j; i++)
		{
			int SelectedElem = SelectedElems[i];
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[SelectedElem].Element);
			// rectangle displaying selection
			draw_list->AddRect(ImVec2(origin.x + Elems[SelectedElem].Pos.x, origin.y + Elems[SelectedElem].Pos.y), ImVec2(origin.x + Elems[SelectedElem].Pos.x + UsedTexture.Width, origin.y + Elems[SelectedElem].Pos.y + UsedTexture.Height), IM_COL32(0, 0, 0, 255));
			draw_list->AddRectFilled(ImVec2(origin.x + Elems[SelectedElem].Pos.x, origin.y + Elems[SelectedElem].Pos.y), ImVec2(origin.x + Elems[SelectedElem].Pos.x + UsedTexture.Width, origin.y + Elems[SelectedElem].Pos.y + UsedTexture.Height), IM_COL32(255, 255, 0, 10));
			// dragging wont change order
			if (CurrentState == ElementDragging && mouseoncanvas)
			{
				int x = Elems[SelectedElem].Pos.x + io.MouseDelta.x;
				int y = Elems[SelectedElem].Pos.y + io.MouseDelta.y;
				if (x < 0) x = 0;
				if (y < 0) y = 0;
				Elems[SelectedElem].Pos = ImVec2(x, y);
			}
			// when SINGLE element clicked it draws on top of the others
			else if (CurrentState==Rest && j < 2)
			{
				ToAdd = { Elems[SelectedElem].Element, Elems[SelectedElem].Pos, Elems[SelectedElem].Type };
				Elems.erase(Elems.begin() + SelectedElem);
				Elems.push_back(ToAdd);
				SelectedElems[i] = -1;
				SelectedElems.push_back(Elems.size() - 1 - i);
				for (int k = 0; k < Links.size(); k++)
				{
					if (Links[k].Elems[0] == SelectedElem) Links[k].Elems[0] = Elems.size() - 1;
					else if (Links[k].Elems[0] > SelectedElem) Links[k].Elems[0]--;
					if (Links[k].Elems[1] == SelectedElem) Links[k].Elems[1] = Elems.size() - 1;
					else if (Links[k].Elems[1] > SelectedElem) Links[k].Elems[1]--;
				}
			}
		}

		auto new_end = std::remove(SelectedElems.begin(), SelectedElems.end(), -1);
		SelectedElems.erase(new_end, SelectedElems.end());
	}

	// io handling
	void CanvasLogic(int clicked_on, ImVec2* SelectionStartPosition)
	{
		// clicked_on: 0 - element, 1 - canvas, 2 - linking point
		ImGuiIO& io = ImGui::GetIO();
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && CurrentState == Rest) // Right-mouse dragging causes canvas dragging
		{
			CurrentState = CanvasDragging;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && CurrentState == CanvasDragging) // Releasing right mouse while dragging will return to rest position
		{
			CurrentState = Rest;
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && clicked_on == 1)
		{
			CurrentState = Selection;
			*SelectionStartPosition = io.MousePos;
			SelectedElems.clear();
		}
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && clicked_on == 0)
		{
			CurrentState = ElementDragging;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && clicked_on != 2)
		{
			CurrentState = Rest;
		}
		switch (CurrentState)
		{
		case CanvasDragging:
			CurrentState = CanvasDragging;
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
			if (scrolling.x > 0.0f) scrolling.x = 0.0f;
			if (scrolling.y > 0.0f) scrolling.y = 0.0f;
			break;
		case Selection:
			draw_list->AddRect(*SelectionStartPosition, io.MousePos, IM_COL32(0, 0, 0, 255));
			draw_list->AddRectFilled(*SelectionStartPosition, io.MousePos, IM_COL32(0, 0, 0, 15));
			break;
		}
	}

	// context menu used for canvas
	void AddCanvasContextMenu()
	{
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		static std::vector<LinkOnCanvas> LinksBuffer;
		if (drag_delta.x == 0.0f && drag_delta.y == 0.0f && CurrentState == Rest)
			ImGui::OpenPopupOnItemClick("contextworkspace", ImGuiPopupFlags_MouseButtonRight);

		if (ImGui::BeginPopup("contextworkspace"))
		{
			if (ImGui::MenuItem(u8"Удалить", NULL, false, SelectedElems.size() > 0))
			{
				int j = SelectedElems.size();
				for (int i = j - 1; i >= 0; i--)
				{
					int SelectedElem = SelectedElems[i];
					Elems.erase(Elems.begin() + SelectedElem);
					std::vector<LinkOnCanvas>::iterator k = Links.begin();
					while (k != Links.end())
					{
						if ((*k).Elems[0] == SelectedElem || (*k).Elems[1] == SelectedElem) k = Links.erase(k);
						else
						{
							if ((*k).Elems[0] > SelectedElem) (*k).Elems[0]--;
							if ((*k).Elems[1] > SelectedElem) (*k).Elems[1]--;
							++k;
						}
					}
				}
				SelectedElems.clear();
			}
			if (ImGui::MenuItem(u8"Копировать", NULL, false, SelectedElems.size() > 0))
			{
				CopyBuffer.clear();
				LinksBuffer.clear();
				std::vector<int> Positions;
				for (int i = 0; i < SelectedElems.size(); i++)
				{
					CopyBuffer.push_back(Elems[SelectedElems[i]]);
					Positions.push_back(SelectedElems[i]);
				}
				for (int k = 0; k < Links.size(); k++)
				{
						int Elem1 = find(Positions.begin(), Positions.end(), Links[k].Elems[0]) - Positions.begin();
						int Elem2 = find(Positions.begin(), Positions.end(), Links[k].Elems[1]) - Positions.begin();
						if (Elem1 < Positions.size() && Elem2 < Positions.size())
						LinksBuffer.push_back({ {Links[k].Points[0], Links[k].Points[1]},{Elem1, Elem2} });
				}
			}
			if (ImGui::MenuItem(u8"Удалить связи", NULL, false, SelectedElems.size() > 0))
			{
				int j = SelectedElems.size();
				for (int i = j - 1; i >= 0; i--)
				{
					std::vector<LinkOnCanvas>::iterator k = Links.begin();
					while (k != Links.end())
					{
						int SelectedElem = SelectedElems[i];
						if ((*k).Elems[0] == SelectedElem || (*k).Elems[1] == SelectedElem) k = Links.erase(k);
						else
						{
							++k;
						}
					}
				}
			}
			if (ImGui::MenuItem(u8"Вставить", NULL, false, CopyBuffer.size() > 0))
			{
				SelectedElems.clear();
				ImVec2 min = ImVec2(-1, -1);
				int ElemsSize = Elems.size();
				for (int i = 0; i < CopyBuffer.size(); i++)
				{
					if (min.x == -1 || CopyBuffer[i].Pos.x < min.x) min.x = CopyBuffer[i].Pos.x;
					if (min.y == -1 || CopyBuffer[i].Pos.y < min.y) min.y = CopyBuffer[i].Pos.y;
				}
				for (int i = 0; i < CopyBuffer.size(); i++)
				{
					Elems.push_back({ CopyBuffer[i].Element, ImVec2(CopyBuffer[i].Pos.x - min.x + MousePos.x, CopyBuffer[i].Pos.y - min.y + MousePos.y), CopyBuffer[i].Type });
					SelectedElems.push_back(Elems.size() - 1);
				}
				for (int k = 0; k < LinksBuffer.size(); k++)
				{
					Links.push_back({ {LinksBuffer[k].Points[0], LinksBuffer[k].Points[1]},{ElemsSize + LinksBuffer[k].Elems[0], ElemsSize + LinksBuffer[k].Elems[1]}});
				}
			}
			ImGui::EndPopup();
		}
	}

	// draw possible objects in elemets window, should be changed later
	void ElementsDrawObjects()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15.0f, 15.0f)); // spacing between buttons
		for (int i = 0; i < ElementsData::NumberOfElements(); i++)
		{
			Texture Tex = ElementsData::GetElementTexture(i);
			ImGui::SameLine(); // used to update GetCursorPos()
			// Should button be in same line?
			if (ImGui::GetStyle().ItemSpacing.x + Tex.Width > ImGui::GetContentRegionAvail().x)
			{
				ImGui::NewLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ImGui::GetStyle().ItemSpacing.x, ImGui::GetCursorPos().y)); // space between rows
			}
			ImGui::PushID(i); // BeginDragDropSource() works correctly with elements with different id;
			ImGui::ImageButton("Element", (void*)Tex.Payload, ImVec2(Tex.Width, Tex.Height));
			// drag'n'drop source
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Element", &i, sizeof(int), ImGuiCond_Once);
				//ImGui::Text(ElementNames[i]);
				ImGui::ImageButton("Element", (void*)Tex.Payload, ImVec2(Tex.Width, Tex.Height));
				ImGui::EndDragDropSource();
			}
			ImGui::PopID();
		}
		ImGui::PopStyleVar();
	}

	// unfinished, should setup params window
	void ParamsInitialization()
	{
		if (SelectedElems.size() == 1)
		{
			if (Elems[SelectedElems[0]].Element % 2 == 1)
			{
				static char str0[128] = "";
				ImGui::InputText("param-text", str0, IM_ARRAYSIZE(str0));
			}
			else
			{
				static int i0 = 123;
				ImGui::InputInt("param-int", &i0);
			}
		}
	}

	// Add element on canvas
	void AddElement(const char * name, float x, float y)
	{
		int j = -1;
		for (int i = 0; i < ScenariosEditorElementsData::ElementsData::NumberOfElements(); i++)
		{ 
			if (!strcmp(ScenariosEditorElementsData::ElementsData::GetElementName(i), name))
			{
				j = i;
				break;
			}
		}
		Elems.push_back(ElementOnCanvas{j, ImVec2(x,y), ScenariosEditorElementsData::ElementsData::GetElementType(j)});
	}
	
	void AddLink(int element_a_index, int element_b_index, int element_a_point, int element_b_point)
	{
		Links.push_back({ {element_a_point, element_b_point} , {element_a_index, element_b_index } });
	}

	const char* GetNameOfElementOnCanvas(int index)
	{
		return ElementsData::GetElementName(Elems[index].Element);
	}
}