//
// ImGui-based UI functions
//

/*

Known problems:

-Config
-Highlighting (static struct of colors, link)
-Should check space on disk
-CTRL+TAB main menu

*/

#include <iomanip>
#include <tchar.h>
#include <iostream>
#include <vector>
#include <d3d11.h>
#include "imgui.h"

#include "xmlhandling.h"
#include "render.h"
#include "ElementsData.h"
#include "ScenarioElement.h"
#include "OpenFileDialog.h"
#include "misc/cpp/imgui_stdlib.h"
#include "ScenarioElementsStorage.h"
#include "ScenarioStorage.h"
#include "SaveFileDialog.h"
#include "CanvasPositioning.h"
#include <sstream>

using namespace ScenariosEditorElementsData;
using namespace ScenariosEditorScenarioElement;
using namespace ScenariosEditorCanvasPositioning;

namespace ScenariosEditorGUI {

	// State machine
	enum CanvasState
	{
		Rest,
		Selection,
		ElementDragging,
		CanvasDragging
	};

	static CanvasState CurrentState = Rest;

	// XML Data
	static ScenariosEditorXML::ScenariosDOM Model;

	// ImGui data
	ImDrawList* canvas_draw_list; // can be put out of global later

	// Helper data
	struct ElementOnCanvas
	{
		int Element;
		ImVec2 Pos;
		int Type;
		std::shared_ptr<ScenarioElement> ElementInStorage;
	};

	struct LinkOnCanvas
	{
		int Points[2];
		int Elems[2];
	};

	static ImVec2 scrolling(0.0f, 0.0f); // current scrolling, can be put out of global later
	static ImVec2 MousePosInCanvas, origin, MousePos;
	static int SelectedElemGUI = -1;
	static float CanvasZoom = 1;

	static std::vector<ElementOnCanvas> Elems;
	static std::vector<LinkOnCanvas> Links;
	static std::vector<int> SelectedElems;
	static std::wstring CurrentFile = L"";

	static bool SearchOpen = false;
	static bool MapOpen = false;

	void PreLoopSetup();
	void MainLoop();

	void DrawMenu();
	void MakeCanvas(const ImGuiViewport* viewport, ImGuiIO& io);
	void DrawCanvas(const ImGuiViewport* viewport, ImGuiIO& io);
	void DrawScenariosSection(const ImGuiViewport* viewport);
	void DrawElementsSection(const ImGuiViewport* viewport);
	void DrawParamsSection(const ImGuiViewport* viewport);
	void CanvasLogic(int hover_on, ImVec2* SelectionStartPosition, ImGuiIO& io);
	void SearchWindow();
	void DoSearch(int SearchType, int SearchElement, std::string SearchAttribute, std::string SearchField);
	void MapWindow();

	// Helper functions
	void ClearElements();
	void ElementsMakeObjects();
	void AddCanvasContextMenu();
	void AddDragAndDropReciever();
	void AddElementGUI(int Element, ImVec2 Pos, int Type, std::shared_ptr<ScenarioElement> CopyOrigin);
	void AddLinkGUI(int PointA, int PointB, int ElemA, int ElemB);
	std::vector<ElementOnCanvas>::iterator DeleteElementGUI(std::vector<ElementOnCanvas>::iterator iter);
	std::vector<LinkOnCanvas>::iterator DeleteLinkGUI(std::vector<LinkOnCanvas>::iterator iter);
	std::vector<const char*> GetElementAttributeNames(ScenariosEditorScenarioElement::ScenarioElement* Elem);
	void RemoveSelectedScenario(std::string GUID);
	void DoubleSelectedScenario(std::string GUID);
	static void HelpMarker(const char* desc);
	void CanvasDrawLinking();
	void CanvasLinkingLogic();
	void CanvasDrawElems();
	void CanvasDrawCaption();
	void CanvasDrawSelectedElems();
	void CanvasElemsLogic(int* hover_on, ImVec2* SelectionStartPosition);
	void CanvasSelectedElemsLogic(ImGuiIO& io);
	void ParamsInitialization();
	ImVec2 GetLinkingPointLocation(int Elem, int Point);


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

		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddChar(0x2116);                               // Add a specific character
		builder.AddChar(0x2013);
		builder.AddChar(0x202F);
		builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic()); // Add one of the default ranges
		builder.BuildRanges(&ranges);                          // Build the final result (ordered ranges with all the unique characters submitted)

		io.Fonts->AddFontFromFileTTF(u8"C:/Users/VR/Desktop/projects/SimulatorsEditor/src/editor/LiberationSans.ttf", 22.0f, NULL, ranges.Data);
		io.Fonts->Build();
		ElementsData::Initialization();
		ScenarioEditorScenarioStorage::ClearScenarioStorage();

		SetCanvasScrollingRef(&scrolling);
		SetDefaulScreenPos();
	}

	// Main loop
	void MainLoop()
	{
		bool done = false;
		while (!done)
		{
			ImGuiIO io = ImGui::GetIO();
			if (ScenariosEditorRender::HandleMessages() || done)
				break;
			ScenariosEditorRender::StartFrame();
			const ImGuiViewport* viewport = ImGui::GetMainViewport();
			// Menu
			DrawMenu();
			// Window
			MakeCanvas(viewport, io);
			DrawScenariosSection(viewport);
			DrawElementsSection(viewport);
			DrawParamsSection(viewport);
			SearchWindow();
			MapWindow();
			ImGui::ShowDemoWindow(); // remove later
			ScenariosEditorRender::EndFrame();
		}
	}

	// Menu
	void DrawMenu()
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu(u8"Файл"))
		{
			if (ImGui::MenuItem(u8"Открыть", "Ctrl+O"))
			{
				const wchar_t* File = ScenarioEditorFileDialog::OpenFileDialog();
				if (size_t(File) != 0)
				{
					if (Model.CheckFile(File))
					{
						ScenariosEditorScenarioElement::ClearScenarioElementStorage();
						ClearElements();
						Model.LoadFrom(File);
						CurrentFile = File;
						SelectedElemGUI = -1;
					}
					else MessageBoxW(NULL, L"При попытке открыть файл возникла ошибка", L"Ошибка", MB_OK);
				}
			}
			//if (ImGui::MenuItem(u8"Недавние файлы..."))
			//{
			//    //
			//    ImGui::EndMenu();
			//}
			if (ImGui::MenuItem(u8"Сохранить", "Ctrl+S", false, CurrentFile != L""))
			{
				Model.SaveTo(CurrentFile.c_str());
			}
			if (ImGui::MenuItem(u8"Сохранить как..."))
			{
				const wchar_t* File = ScenarioEditorFileDialog::SaveFileDialog();
				if (size_t(File) != 0)
				{
					Model.SaveTo(File);
					CurrentFile = File;
				}
			}
			ImGui::EndMenu();

		}
		if (ImGui::BeginMenu(u8"Инструменты"))
		{
			if (ImGui::MenuItem(u8"Поиск", "CTRL+F", &SearchOpen))
			{
				SearchOpen != SearchOpen;
			}
			if (ImGui::MenuItem(u8"Карта", "CTRL+M", &MapOpen))
			{
				MapOpen != MapOpen;
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	// Clear existing elements
	void ClearElements()
	{
		SelectedElems.clear();
		Elems.clear();
		Links.clear();
		CurrentState = Rest;
	}


	// Canvas section
	void MakeCanvas(const ImGuiViewport* viewport, ImGuiIO& io)
	{
		DrawCanvas(viewport, io);

		int hover_on = ImGui::IsItemHovered(); // used for logic
		// Add context menu
		AddCanvasContextMenu();

		// Canvas is drag'n'drop reciever
		AddDragAndDropReciever();

		static ImVec2 SelectionStartPosition; // used for logic

		CanvasDrawCaption();

		CanvasElemsLogic(&hover_on, &SelectionStartPosition);

		CanvasDrawLinking();
		CanvasLinkingLogic();

		CanvasDrawSelectedElems();
		CanvasSelectedElemsLogic(io);
		CanvasDrawElems();
		CanvasLogic(hover_on, &SelectionStartPosition, io);
		ImGui::End();
	}

	void AddElementGUI(int Element, ImVec2 Pos, int Type, std::shared_ptr<ScenarioElement> CopyOrigin = nullptr)
	{
		if (ScenariosEditorElementsData::ElementsData::GetElementName(Element) == u8"Start")
		{
			for (ElementOnCanvas Elem : Elems)
				if (ScenariosEditorElementsData::ElementsData::GetElementName(Elem.Element) == u8"Start") return;
			// Only one "Start" element should be allowed
		}
		unsigned int type = ElementsData::GetElementType(Element);
		int pin_count;
		for (pin_count = 0; type > 0; type >>= 1)
			if (type & 1) pin_count++;
		CopyOrigin == nullptr ?
			Elems.push_back({ Element, Pos, ElementsData::GetElementType(Element),
				ScenariosEditorScenarioElement::AddScenarioElementStorageElement(ScenariosEditorElementsData::ElementsData::GetElementName(Element), Pos.x, Pos.y, 0.00f, pin_count) })
			:
			Elems.push_back({ Element, Pos, ElementsData::GetElementType(Element),
				ScenariosEditorScenarioElement::AddScenarioElementStorageElement(ScenariosEditorElementsData::ElementsData::GetElementName(Element), Pos.x, Pos.y, 0.00f, pin_count, CopyOrigin) });
	}

	void AddLinkGUI(int PointA, int PointB, int ElemA, int ElemB)
	{
		std::vector<int> ToAdd;

		int index = ScenariosEditorScenarioElement::GetPinIndex((*Elems[ElemA].ElementInStorage).getElementName().c_str(), PointA);
		ToAdd.push_back((*Elems[ElemA].ElementInStorage).pins[index]);
		index = ScenariosEditorScenarioElement::GetPinIndex((*Elems[ElemB].ElementInStorage).getElementName().c_str(), PointB);
		ToAdd.push_back((*Elems[ElemB].ElementInStorage).pins[index]);
		ScenariosEditorScenarioElement::AddScenarioElementStorageLink(ToAdd);
		Links.push_back({ {PointA, PointB},{ElemA, ElemB} });
	}

	std::vector<ElementOnCanvas>::iterator DeleteElementGUI(std::vector<ElementOnCanvas>::iterator iter)
	{
		ScenariosEditorScenarioElement::RemoveScenarioElementStorageElement((*iter).ElementInStorage);
		return Elems.erase(iter);
	}

	std::vector<LinkOnCanvas>::iterator DeleteLinkGUI(std::vector<LinkOnCanvas>::iterator iter)
	{
		std::vector<int> ToRem;
		int index = ScenariosEditorScenarioElement::GetPinIndex((*Elems[(*iter).Elems[0]].ElementInStorage).getElementName().c_str(), (*iter).Points[0]);
		ToRem.push_back((*Elems[(*iter).Elems[0]].ElementInStorage).pins[index]);
		index = ScenariosEditorScenarioElement::GetPinIndex((*Elems[(*iter).Elems[1]].ElementInStorage).getElementName().c_str(), (*iter).Points[1]);
		ToRem.push_back((*Elems[(*iter).Elems[1]].ElementInStorage).pins[index]);
		ScenariosEditorScenarioElement::RemoveScenarioElementStorageLink(ToRem);
		return Links.erase(iter);
	}

	void AddDragAndDropReciever()
	{
		// canvas is drag'n'drop reciever
		if (ImGui::BeginDragDropTarget()) {

			auto payload = ImGui::AcceptDragDropPayload("Element");
			if (payload != NULL) {
				int ElementNum = *(int*)payload->Data;
				int x = MousePosInCanvas.x - (ElementsData::GetElementTexture(ElementNum, CanvasZoom).Width / 2) / CanvasZoom;
				int y = MousePosInCanvas.y - (ElementsData::GetElementTexture(ElementNum, CanvasZoom).Height / 2) / CanvasZoom;
				AddElementGUI(ElementNum, ImVec2(x, y), ElementsData::GetElementType(ElementNum));
			}
			ImGui::EndDragDropTarget();
		}
	}

	// Context menu used for canvas
	void AddCanvasContextMenu() // TODO: split function
	{
		static std::vector<ElementOnCanvas> CopyBuffer;
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		static std::vector<LinkOnCanvas> LinksBuffer;
		if (drag_delta.x == 0.0f && drag_delta.y == 0.0f && CurrentState == Rest)
			ImGui::OpenPopupOnItemClick("contextworkspace", ImGuiPopupFlags_MouseButtonRight);

		if (ImGui::BeginPopup("contextworkspace"))
		{
			if (ImGui::MenuItem(u8"Удалить", NULL, false, SelectedElems.size() > 0))
			{
				for (int i = SelectedElems.size() - 1; i >= 0; i--)
				{
					int SelectedElem = SelectedElems[i];
					std::vector<LinkOnCanvas>::iterator k = Links.begin();
					while (k != Links.end())
					{
						if ((*k).Elems[0] == SelectedElem || (*k).Elems[1] == SelectedElem) k = DeleteLinkGUI(k);
						else
						{
							if ((*k).Elems[0] > SelectedElem) (*k).Elems[0]--;
							if ((*k).Elems[1] > SelectedElem) (*k).Elems[1]--;
							++k;
						}
					}
					DeleteElementGUI(Elems.begin() + SelectedElem);
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
					if (ScenariosEditorElementsData::ElementsData::GetElementName(Elems[SelectedElems[i]].Element) == "Start") continue;
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
				for (int i = SelectedElems.size() - 1; i >= 0; i--)
				{
					std::vector<LinkOnCanvas>::iterator k = Links.begin();
					while (k != Links.end())
					{
						int SelectedElem = SelectedElems[i];
						if ((*k).Elems[0] == SelectedElem || (*k).Elems[1] == SelectedElem) k = DeleteLinkGUI(k);
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
				ImVec2 min = ImVec2(INT_MAX, INT_MAX);
				int ElemsSize = Elems.size();
				for (int i = 0; i < CopyBuffer.size(); i++)
				{
					if (min.x == INT_MAX || CopyBuffer[i].Pos.x < min.x) min.x = CopyBuffer[i].Pos.x;
					if (min.y == INT_MAX || CopyBuffer[i].Pos.y < min.y) min.y = CopyBuffer[i].Pos.y;
				}
				for (int i = 0; i < CopyBuffer.size(); i++)
				{
					AddElementGUI(CopyBuffer[i].Element,
						ImVec2(CopyBuffer[i].Pos.x - min.x + MousePosInCanvas.x, CopyBuffer[i].Pos.y - min.y + MousePosInCanvas.y),
						CopyBuffer[i].Type, CopyBuffer[i].ElementInStorage);
					SelectedElems.push_back(Elems.size() - 1);
				}
				for (int k = 0; k < LinksBuffer.size(); k++)
				{
					AddLinkGUI(LinksBuffer[k].Points[0], LinksBuffer[k].Points[1],
						ElemsSize + LinksBuffer[k].Elems[0], ElemsSize + LinksBuffer[k].Elems[1]);
				}
			}
			ImGui::EndPopup();
		}
	}

	// Give linking points coordinates depending on elem's type and texture
	ImVec2 GetLinkingPointLocation(int Elem, int Point)
	{
		switch (Point)
		{
		case 0: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Width / 2,
			origin.y + Elems[Elem].Pos.y * CanvasZoom
		); break;
		case 1: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Width,
			origin.y + Elems[Elem].Pos.y * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Height / 2
		); break;
		case 2: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Width / 2,
			origin.y + Elems[Elem].Pos.y * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Height
		); break;
		case 3: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom,
			origin.y + Elems[Elem].Pos.y * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Height / 2
		); break;
		}
	}

	// Add buttons which represent linking points
	void AddLinkingPoint(int Point, int* ClickedElem, int* ClickedType, int Elem)
	{
		// out of state machine for logic purposes
		static bool IsLinking = false;
		canvas_draw_list->AddCircleFilled(GetLinkingPointLocation(Elem, Point), 5.0f, IM_COL32(0, 0, 0, 255));
		ImGui::SetCursorScreenPos(ImVec2(GetLinkingPointLocation(Elem, Point).x - 5.0f, GetLinkingPointLocation(Elem, Point).y - 5.0f));
		ImGui::InvisibleButton("LinkingPoint", ImVec2(10.0f, 10.0f), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		ImGui::SetItemAllowOverlap();
		if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
		{
			if (!IsLinking)
			{
				*ClickedElem = Elem;
				*ClickedType = Point;
			}
			else
			{
				if (*ClickedType != Point || *ClickedElem != Elem)
				{
					AddLinkGUI(*ClickedType, Point, *ClickedElem, Elem);
				}
			}
			IsLinking = !IsLinking;
		}
		if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
		{

		}
		if (IsLinking)
		{
			SelectedElems.clear();
			canvas_draw_list->AddLine(GetLinkingPointLocation(*ClickedElem, *ClickedType), MousePos, IM_COL32(0, 0, 0, 255));
		}
	}

	// Sub-canvas drawing functions
	// Draw canvas
	void DrawCanvas(const ImGuiViewport* viewport, ImGuiIO& io)
	{
		// Canvas size and style
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x / 4, viewport->WorkPos.y));
		ImGui::SetNextWindowSize(ImVec2(3 * viewport->WorkSize.x / 4, viewport->WorkSize.y));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("MainWorkspace", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus | ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::PopStyleVar();

		// Canvas positioning
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		MousePos = io.MousePos;
		canvas_draw_list = ImGui::GetWindowDrawList();
		canvas_draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(240, 240, 240, 0));
		// Window itself doesn't trigger io mouse actions, invisible button does
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		ImGui::SetItemAllowOverlap();
		static ImVec2 shift = { 0, 0 };
		if (ImGui::IsItemHovered())
		{
			if (ImGui::GetIO().MouseWheel != 0)
			{
				float oldzoom = CanvasZoom;
				CanvasZoom += ImGui::GetIO().MouseWheel * 0.05f;
				if (CanvasZoom > 2) CanvasZoom = 2;
				else if (CanvasZoom < 0.5) CanvasZoom = 0.5f;
				
				if (oldzoom != CanvasZoom)
				{
					shift.x -= (canvas_sz.x * CanvasZoom - canvas_sz.x * oldzoom) / 2.0f;
					shift.y -= (canvas_sz.y * CanvasZoom - canvas_sz.y * oldzoom) / 2.0f;
				}
				std::cout << CanvasZoom;
			}
		}
		canvas_draw_list->PopClipRect();
		origin = ImVec2(canvas_p0.x + scrolling.x * CanvasZoom + shift.x, canvas_p0.y + scrolling.y * CanvasZoom + shift.y);
		// Draw grid
		canvas_draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		{
			const float GRID_STEP = 79.0f * CanvasZoom;
			for (float x = fmodf(origin.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
				canvas_draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 119));
			for (float y = fmodf(origin.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
				canvas_draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 119));
		}
		MousePosInCanvas = ImVec2((io.MousePos.x - origin.x) / CanvasZoom, (io.MousePos.y - origin.y) / CanvasZoom);
		MousePos = io.MousePos;
	}
	// Draw elems
	void CanvasDrawElems()
	{
		for (int i = 0; i < Elems.size(); i++)
		{
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element, CanvasZoom);
			canvas_draw_list->AddImage(
				(void*)UsedTexture.Payload, ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom, origin.y + Elems[i].Pos.y * CanvasZoom),
				ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom + UsedTexture.Width, origin.y + Elems[i].Pos.y * CanvasZoom + UsedTexture.Height ));

		}
	}
	// Draw rectangle over selected elems
	void CanvasDrawSelectedElems()
	{
		for (int i = 0; i < SelectedElems.size(); i++)
		{
			int SelectedElem = SelectedElems[i];
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[SelectedElem].Element, CanvasZoom);
			// rectangle displaying selection
			canvas_draw_list->AddRect(
				ImVec2(origin.x + Elems[SelectedElem].Pos.x * CanvasZoom, origin.y + Elems[SelectedElem].Pos.y * CanvasZoom),
				ImVec2(origin.x + Elems[SelectedElem].Pos.x * CanvasZoom + UsedTexture.Width, origin.y + Elems[SelectedElem].Pos.y * CanvasZoom + UsedTexture.Height),
				IM_COL32(0, 0, 0, 255));
			canvas_draw_list->AddRectFilled(
				ImVec2(origin.x + Elems[SelectedElem].Pos.x * CanvasZoom, origin.y + Elems[SelectedElem].Pos.y * CanvasZoom),
				ImVec2(origin.x + Elems[SelectedElem].Pos.x * CanvasZoom + UsedTexture.Width, origin.y + Elems[SelectedElem].Pos.y * CanvasZoom + UsedTexture.Height),
				IM_COL32(255, 255, 0, 10));
		}
	}
	// Draw links
	void CanvasDrawLinking()
	{
		for (int j = 0; j < Links.size(); j++)
		{
			canvas_draw_list->AddLine(
				GetLinkingPointLocation(Links[j].Elems[0], Links[j].Points[0]),
				GetLinkingPointLocation(Links[j].Elems[1], Links[j].Points[1]),
				IM_COL32(0, 0, 0, 255)
			);
		}
	}
	// Draw caption
	void CanvasDrawCaption()
	{
		float font_size = 20;
		float wrap_size = 400;
		float shift_x = 200;
		float shift_y = 20;
		for (ElementOnCanvas Elem : Elems)
		{
			//if (origin.y + Elem.Pos.y > 0)
				//canvas_draw_list->AddText(ImGui::GetFont(), font_size, ImVec2(origin.x + Elem.Pos.x + shift_x, origin.y + Elem.Pos.y + shift_y), IM_COL32(0, 0, 0, 255), (Elem.ElementInStorage)->caption.c_str(), (const char*)0, wrap_size);
		}
	}


	// Logic functions, using state machine, used to handle io actions
	//
	// Changes state machine, handles canvas-related actions
	void CanvasLogic(int clicked_on, ImVec2* SelectionStartPosition, ImGuiIO& io)
	{
		// clicked_on: 0 - offcanvas, 1 - canvas, 2 - linking point, 3 - element
		// Right-mouse dragging causes canvas dragging
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Right) && CurrentState == Rest)
		{
			CurrentState = CanvasDragging;
		}
		// Releasing right mouse while dragging will return to rest position
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Right) && CurrentState == CanvasDragging)
		{
			CurrentState = Rest;
		}
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && clicked_on == 1 && CurrentState == Rest)
		{
			CurrentState = Selection;
			*SelectionStartPosition = io.MousePos;
			SelectedElems.clear();
		}
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && clicked_on == 3 && CurrentState == Rest)
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
			scrolling.x += io.MouseDelta.x / CanvasZoom;
			scrolling.y += io.MouseDelta.y / CanvasZoom;
			break;
		case Selection:
			canvas_draw_list->AddRect(*SelectionStartPosition, io.MousePos, IM_COL32(0, 0, 0, 255));
			canvas_draw_list->AddRectFilled(*SelectionStartPosition, io.MousePos, IM_COL32(0, 0, 0, 15));
			SelectedElems.clear();
			break;
		}
	}
	// Handles elems-depending logic
	void CanvasElemsLogic(int* hover_on, ImVec2* SelectionStartPosition)
	{
		static ImVec2 OldElementPosition;
		for (int i = 0; i < Elems.size(); i++)
		{
			// Create button and handle it io actions
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element, CanvasZoom);
			ImGui::SetCursorScreenPos(ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom, origin.y + Elems[i].Pos.y * CanvasZoom));
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
				*hover_on = 3;
			}
			// Foreach element we determine if it contains in selection
			if (CurrentState == Selection)
			{
				if (Elems[i].Pos.x * CanvasZoom + origin.x + UsedTexture.Width / 2 > (MousePos.x + SelectionStartPosition->x - max(MousePos.x, SelectionStartPosition->x))
					&& Elems[i].Pos.x * CanvasZoom + origin.x + UsedTexture.Height / 2 < (MousePos.x + SelectionStartPosition->x - min(MousePos.x, SelectionStartPosition->x))
					&& Elems[i].Pos.y * CanvasZoom + origin.y + UsedTexture.Width / 2 > (MousePos.y + SelectionStartPosition->y - max(MousePos.y, SelectionStartPosition->y))
					&& Elems[i].Pos.y * CanvasZoom + origin.y + UsedTexture.Height / 2 < (MousePos.y + SelectionStartPosition->y - min(MousePos.y, SelectionStartPosition->y))
					&& std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end()
					)
				{
					SelectedElems.push_back(i);
				}
			}
		}
	}
	//
	void CanvasSelectedElemsLogic(ImGuiIO& io)
	{
		ElementOnCanvas ToAdd;
		// dragging, clicking on elements realization
		int j = SelectedElems.size();
		for (int i = 0; i < j; i++)
		{
			int SelectedElem = SelectedElems[i];
			// dragging wont change order
			if (CurrentState == ElementDragging)
			{
				int x = Elems[SelectedElem].Pos.x + io.MouseDelta.x / CanvasZoom;
				int y = Elems[SelectedElem].Pos.y + io.MouseDelta.y / CanvasZoom;
				Elems[SelectedElem].Pos = ImVec2(x, y);
				ScenariosEditorScenarioElement::UpdateCoordinates(Elems[SelectedElem].ElementInStorage, x, y);
			}
			// when SINGLE element clicked it draws on top of the others
			else if (CurrentState == Rest && j < 2)
			{
				ToAdd = { Elems[SelectedElem].Element, Elems[SelectedElem].Pos, Elems[SelectedElem].Type, Elems[SelectedElem].ElementInStorage };
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
	//
	void CanvasLinkingLogic()
	{
		static int ClickedElem;
		static int ClickedType;
		for (int i = 0; i < Elems.size(); i++)
		{
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
		}
	}

	static void HelpMarker(const char* desc)
	{
		ImGui::TextDisabled("(?)");
		if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort))
		{
			ImGui::BeginTooltip();
			ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
			ImGui::TextUnformatted(desc);
			ImGui::PopTextWrapPos();
			ImGui::EndTooltip();
		}
	}

	// scenarios section
	void DrawScenariosSection(const ImGuiViewport* viewport)
	{
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, (viewport->WorkSize.y / 3) - 29));
		ImGui::Begin(u8"Сценарии", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
		if (ImGui::Button(u8"Создать сценарий"))
		{
			ScenarioEditorScenarioStorage::CreateNewScenario();
		}
		ImGui::SameLine();
		std::vector<std::string> Scenarios = ScenarioEditorScenarioStorage::GetScenarioNames();
		if (SelectedElemGUI > 0 && SelectedElemGUI < Scenarios.size())
		{
			if (ImGui::Button(u8"Дублировать выбранный"))
			{
				DoubleSelectedScenario(ScenarioEditorScenarioStorage::GetActualGUID());
				Scenarios = ScenarioEditorScenarioStorage::GetScenarioNames();
				SelectedElemGUI = Scenarios.size() - 1;

			}
			ImGui::SameLine();
			if (ImGui::Button(u8"Удалить выбранный"))
			{
				RemoveSelectedScenario(ScenarioEditorScenarioStorage::GetActualGUID());
				Scenarios = ScenarioEditorScenarioStorage::GetScenarioNames();
				SelectedElemGUI = 0;
			}
		}

		ImGui::SameLine();
		HelpMarker(
			u8"Имя сценария идентично значению аттрибута \"Caption\" элемента \"Start\"\n"
			"Элемет \"Start\" в сценарии может быть только один");
		static bool ShouldCenter = true;
		ImGui::Checkbox(u8"Центрировать при загрузке сценария", &ShouldCenter);
		const float footer_height_to_reserve = (ImGui::GetStyle().ItemSpacing.y * 2) + ImGui::GetTextLineHeight();
		if (ImGui::BeginChild("ScrollingRegion", ImVec2(0, -footer_height_to_reserve), false, ImGuiWindowFlags_HorizontalScrollbar))
		{
			std::vector<std::vector<std::string>> ScenarioList = ScenarioEditorScenarioStorage::GetScenarios();
			for (int n = 0; n < Scenarios.size(); n++)
			{
				std::string GUID = ScenarioList[n][1];
				std::string item = Scenarios[n];
				if (item == "")
				{
					if (GUID == "")
						item = u8"<Вне сценария>";
					else item = u8"<Без имени>";
				}
				if (ImGui::Selectable((item + u8"##" + GUID).c_str(), SelectedElemGUI == n))
				{
					SelectedElemGUI = n;
					CurrentState = Rest;
					ScenarioEditorScenarioStorage::SetActualScenario(n);
					ScenariosEditorScenarioElement::LoadElements();
					if (ShouldCenter) CenterScenario();
				}
				if (ImGui::IsItemActive() && !ImGui::IsItemHovered())
				{
					int n_next = n + (ImGui::GetMouseDragDelta(0).y < 0.f ? -1 : 1);
					if (n != 0 && n_next >= 1 && n_next < Scenarios.size())
					{
						if (n_next == SelectedElemGUI) SelectedElemGUI = n;
						else if (n == SelectedElemGUI) SelectedElemGUI = n_next;
						ScenarioEditorScenarioStorage::SwapScenario(n, n_next);
						ImGui::ResetMouseDragDelta();
					}
				}
			}
		}
		ImGui::EndChild();
		ImGui::Separator();
		if (CurrentFile == L"")
			ImGui::Text(u8"Файл не выбран");
		else
			ImGui::Text((u8"Выбранный файл: " + std::string(CurrentFile.begin(), CurrentFile.end())).c_str());
		ImGui::End();
	}

	void RemoveSelectedScenario(std::string GUID)
	{
		ScenarioEditorScenarioStorage::RemoveScenario(GUID);
		std::vector<ElementOnCanvas>::iterator k = Elems.begin();
		while (k != Elems.end())
		{
			k = DeleteElementGUI(k);
		}
		ScenarioEditorScenarioStorage::SetActualScenario(0);
		ScenariosEditorScenarioElement::LoadElements();
	}

	void DoubleSelectedScenario(std::string GUID)
	{
		std::vector<ElementOnCanvas> PreviousElems = Elems;
		std::vector<LinkOnCanvas> PreviousLinks = Links;
		ScenarioEditorScenarioStorage::DoubleScenario(GUID);
		ScenarioEditorScenarioStorage::SetActualScenario(ScenarioEditorScenarioStorage::GetScenarios().size() - 1);
		ScenariosEditorScenarioElement::LoadElements();
		std::vector<ElementOnCanvas>::iterator k = PreviousElems.begin();
		while (k != PreviousElems.end())
		{
			AddElementGUI((*k).Element, (*k).Pos, (*k).Type, (*k).ElementInStorage);
			++k;
		}
		std::vector<LinkOnCanvas>::iterator j = PreviousLinks.begin();
		while (j != PreviousLinks.end())
		{
			AddLinkGUI((*j).Points[0], (*j).Points[1], (*j).Elems[0], (*j).Elems[1]);
			++j;
		}
	}

	// elements section
	void DrawElementsSection(const ImGuiViewport* viewport)
	{
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, (viewport->WorkSize.y / 3) - 2));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, viewport->WorkSize.y / 3));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 15));
		ImGui::Begin(u8"Элементы сценария", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
		ImGui::PopStyleVar();
		ElementsMakeObjects();
		ImGui::End();
	}

	// params section
	void DrawParamsSection(const ImGuiViewport* viewport)
	{
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, (2 * viewport->WorkSize.y / 3) - 4));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, (viewport->WorkSize.y / 3) + 33));
		ImGui::Begin(u8"Свойства", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
		ParamsInitialization();
		ImGui::End();
	}

	// Make possible objects in elemets window, should be changed later
	void ElementsMakeObjects()
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

	// fill params window
	void ParamsInitialization()
	{
		if (SelectedElems.size() == 1)
		{
			std::vector<ElementAttribute*> Attributes = (*Elems[SelectedElems[0]].ElementInStorage).GetAttributes();
			if (ImGui::BeginTable("Attributes", 3, ImGuiTableFlags_SizingFixedFit))
			{
				ImGui::TableSetupColumn("Names", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableSetupColumn("Fields", ImGuiTableColumnFlags_WidthStretch);
				ImGui::TableSetupColumn("Hints", ImGuiTableColumnFlags_WidthFixed);
				ImGui::TableNextRow();
				ImGui::TableSetColumnIndex(0);
				ImGui::AlignTextToFramePadding();
				ImGui::Text("Caption");
				ImGui::TableSetColumnIndex(1);
				ImGui::PushItemWidth(ImGui::GetColumnWidth());
				int count = 1;
				for (int j = 0; j < (Elems[SelectedElems[0]].ElementInStorage)->caption.size(); j++)
					if ((Elems[SelectedElems[0]].ElementInStorage)->caption[j] == '\n') count++;
				ImGui::InputTextMultiline("##Caption", &(Elems[SelectedElems[0]].ElementInStorage)->caption, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * (count + 1)));
				if (ScenariosEditorElementsData::ElementsData::GetElementName(Elems[SelectedElems[0]].Element) == "Start")
					ScenarioEditorScenarioStorage::SetActualScenarioName((Elems[SelectedElems[0]].ElementInStorage)->caption);
				ImGui::PopItemWidth();
				ImGui::TableSetColumnIndex(2);
				static const float wrap_width = 200.0f;
				for (int i = 0; i < Attributes.size(); i++)
				{
					std::string label = std::string("##") + (Attributes[i])->Name;
					ImGui::TableNextRow();
					ImGui::TableSetColumnIndex(0);
					ImGui::AlignTextToFramePadding();
					ImGui::Text((Attributes[i])->Name.c_str());
					ImGui::TableSetColumnIndex(1);
					ImGui::PushItemWidth(ImGui::GetColumnWidth());
					int count = 1;
					switch ((Attributes[i])->GetFormat())
					{
					case 0:
						for (int j = 0; j < (Attributes[i])->ValueS.size(); j++)
							if ((Attributes[i])->ValueS[j] == '\n') count++;
						ImGui::InputTextMultiline(label.c_str(), &(Attributes[i])->ValueS, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * (count + 1)));
						//ImGui::InputText(label.c_str(), &(Attributes[i])->ValueS);
						break;
					case 1:
					{
						int State = (Attributes[i])->ValueF;
						const char* items[] = { "False", "True" };
						ImGui::Combo(label.c_str(), &State, items, IM_ARRAYSIZE(items));
						(Attributes[i])->ValueF = State;
					}
					break;
					case 2:
						ImGui::InputFloat(label.c_str(), &(Attributes[i])->ValueF, 0.01f, 1.0f, "%.2f");
						break;
					}
					ImGui::PopItemWidth();
					ImGui::TableSetColumnIndex(2);
					ImGui::PushTextWrapPos(ImGui::GetCursorPos().x + wrap_width);
					ImGui::AlignTextToFramePadding();
					ImGui::Text(ScenariosEditorElementsData::ElementsData::GetAttributeHint((Attributes[i])->Name).c_str(), wrap_width);
					ImGui::PopTextWrapPos();
				}
			}
			ImGui::EndTable();
		}
	}

	void SearchWindow()
	{
		if (SearchOpen)
		{
			static const char* LongestTextLabel = u8"Тип поиска";
			float Spacing = ImGui::CalcTextSize(LongestTextLabel).x + ImGui::GetStyle().WindowPadding.x + ImGui::GetStyle().ItemSpacing.x;
			ImGui::Begin(u8"Поиск");
			static const char* SearchTypes[] = { u8"Среди всех аттрибутов", u8"По определённому элементу" };
			static std::vector<std::vector<std::string>> AttributeNames = ScenariosEditorScenarioElement::GetAllElementsAttributeNames();
			static int CurrentSearchType = 0; // Here we store our selection data as an index.
			static int CurrentElement = 0;
			static int CurrentAttribute = 0;
			const char* TypePreview = SearchTypes[CurrentSearchType];
			ImGui::Text(u8"Тип поиска");
			ImGui::SameLine(Spacing);
			if (ImGui::BeginCombo(u8"##Тип поиска", TypePreview))
			{
				for (int n = 0; n < IM_ARRAYSIZE(SearchTypes); n++)
				{
					const bool is_selected = (CurrentSearchType == n);
					if (ImGui::Selectable(SearchTypes[n], is_selected))
						CurrentSearchType = n;
					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			if (CurrentSearchType == 1)
			{
				
				static const std::vector<const char*> Elements = { "Uzel", "Start", "Clear", "Message", "Sound", "Script", "Pilon", "Arrow",
										"Pause", "Push", "Select", "Outcome", "Answer", "Variable value", "Random", "Danger" };
				const char* ElementPreview = Elements[CurrentElement];
				ImGui::Text(u8"Элемент");
				ImGui::SameLine(Spacing);
				if (ImGui::BeginCombo(u8"##Элемент", ElementPreview))
				{
					for (int n = 0; n < Elements.size(); n++)
					{
						const bool is_selected = (CurrentElement == n);
						if (ImGui::Selectable(Elements[n], is_selected))
						{
							CurrentElement = n;
							CurrentAttribute = 0;
						}
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
				
				
				const char* AttributePreview = AttributeNames[CurrentElement][CurrentAttribute].c_str();
				ImGui::Text(u8"Аттрибут");
				ImGui::SameLine(Spacing);
				if (ImGui::BeginCombo(u8"##Аттрибут", AttributePreview))
				{
					for (int n = 0; n < AttributeNames[CurrentElement].size(); n++)
					{
						const bool is_selected = (CurrentAttribute == n);
						if (ImGui::Selectable(AttributeNames[CurrentElement][n].c_str(), is_selected))
							CurrentAttribute = n;
						if (is_selected)
							ImGui::SetItemDefaultFocus();
					}
					ImGui::EndCombo();
				}
			}
			
			static std::string SearchField = "";
			int count = 1;
			for (int j = 0; j < SearchField.size(); j++)
				if (SearchField[j] == '\n') count++;
			if (CurrentSearchType != 1 || CurrentAttribute != 1)
			{
				ImGui::Text(u8"Найти:");
				ImGui::SameLine(Spacing);
				ImGui::InputTextMultiline(u8"##Найти", &SearchField, ImVec2(ImGui::CalcItemWidth(), ImGui::GetTextLineHeight() * (count + 1)));
			}
			DoSearch(CurrentSearchType, CurrentElement, AttributeNames[CurrentElement][CurrentAttribute], SearchField);
			ImGui::End();
		}
	}

	void DoSearch(int SearchType, int SearchElement, std::string SearchAttribute, std::string SearchField)
	{
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		//	Draw list now won't check if elem is out of canvas (called after End())
		if (SearchType == 0)
		{
			if (SearchField.size() == 0) return;
			for (ElementOnCanvas Elem : Elems)
			{
				bool ShouldSelect = false;
				if ((*Elem.ElementInStorage).caption.find(SearchField) != std::string::npos)
				{
					ShouldSelect = true;
				}
				else
				{
					for (ElementAttribute* Attribute : (*Elem.ElementInStorage).GetAttributes())
					{
						if (Attribute->ValueS.find(SearchField) != std::string::npos)
						{
							ShouldSelect = true;
							break;
						}
						std::stringstream Float;
						Float << std::fixed << std::setprecision(2) << Attribute->ValueF;
						if (Float.str().find(SearchField) != std::string::npos)
						{
							ShouldSelect = true;
							break;
						}
					}
				}
				if (ShouldSelect)
				{
					Texture UsedTexture = ElementsData::GetElementTexture(Elem.Element, CanvasZoom);
					float x = origin.x + Elem.Pos.x * CanvasZoom - 10 > viewport->WorkSize.x / 4 ? origin.x + Elem.Pos.x * CanvasZoom - 10 : viewport->WorkSize.x / 4;
					if (x < origin.x + Elem.Pos.x * CanvasZoom + UsedTexture.Width + 10)
					canvas_draw_list->AddRectFilled(
						ImVec2(x, origin.y + Elem.Pos.y * CanvasZoom - 10),
						ImVec2(origin.x + Elem.Pos.x * CanvasZoom + UsedTexture.Width + 10, origin.y + Elem.Pos.y * CanvasZoom + UsedTexture.Height + 10),
						IM_COL32(0, 255, 64, 50));
				}
			}
		}
		else // SearchType == 1
		{
			if (SearchAttribute != u8"<Найти все>" && SearchField.size() == 0) return;
			for (ElementOnCanvas Elem : Elems)
			{
				if (Elem.Element != SearchElement)
				{
					continue;
				}
				bool ShouldSelect = false;
				if (SearchAttribute == u8"<Найти все>")
				{
					ShouldSelect = true;
				}
				else if (SearchAttribute == u8"Caption" || SearchAttribute == u8"<Среди всех аттрибутов>")
				{
					ShouldSelect = (*Elem.ElementInStorage).caption.find(SearchField) != std::string::npos;
				}
				else
				{
					for (ElementAttribute* Attribute : (*Elem.ElementInStorage).GetAttributes())
					{
						if ((SearchAttribute == u8"<Среди всех аттрибутов>") || (Attribute->Name == SearchAttribute))
						{
							if (Attribute->GetFormat() == 0)
							{
								if (Attribute->ValueS.find(SearchField) != std::string::npos)
								{
									ShouldSelect = true;
									break;
								}
							}
							else {
								std::stringstream Float;
								Float << std::fixed << std::setprecision(2) << Attribute->ValueF;
								if (Float.str().find(SearchField) != std::string::npos)
								{
									ShouldSelect = true;
									break;
								}
							}
						}
					}
				}
				if (ShouldSelect)
				{
					Texture UsedTexture = ElementsData::GetElementTexture(Elem.Element, CanvasZoom);
					float x = origin.x + Elem.Pos.x * CanvasZoom - 10 > viewport->WorkSize.x / 4 ? origin.x + Elem.Pos.x * CanvasZoom - 10 : viewport->WorkSize.x / 4;
					if (x < origin.x + Elem.Pos.x * CanvasZoom + UsedTexture.Width + 10)
						canvas_draw_list->AddRectFilled(
							ImVec2(x, origin.y + Elem.Pos.y * CanvasZoom - 10),
							ImVec2(origin.x + Elem.Pos.x * CanvasZoom + UsedTexture.Width + 10, origin.y + Elem.Pos.y * CanvasZoom + UsedTexture.Height + 10),
							IM_COL32(0, 255, 64, 50));
				}
			}
		}
	}

	void MapWindow()
	{
		if (MapOpen)
		{
			ImGui::Begin(u8"Карта");
			ImGui::End();
		}
	}

	// .h file helper functions realisation
	// 
	// Add element on canvas
	void AddElement(const char* name, float x, float y, std::shared_ptr<ScenarioElement> actual_element)
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
		Elems.push_back(ElementOnCanvas{ j, ImVec2(x,y), ScenariosEditorElementsData::ElementsData::GetElementType(j), actual_element });
	}
	// Add link on canvas
	void AddLink(int element_a_index, int element_b_index, int element_a_point, int element_b_point)
	{
		Links.push_back({ {element_a_point, element_b_point} , {element_a_index, element_b_index } });
	}

	const char* GetNameOfElementOnCanvas(int index)
	{
		return ElementsData::GetElementName(Elems[index].Element);
	}
	int GetNumOfElement(std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> Elem)
	{
		for (int i = 0; i < Elems.size(); i++)
		{
			if (Elems[i].ElementInStorage == Elem)
				return i;
		}
		return -1;
	}
}