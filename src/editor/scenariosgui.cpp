//
// ImGui-based UI functions
//

#include "SavedSettings.h"
#include "xmlhandling.h"
#include "render.h"
#include "ElementsData.h"
#include "OpenFileDialog.h"
#include "ScenarioElementsStorage.h"
#include "ScenarioStorage.h"
#include "SaveFileDialog.h"
#include "CanvasPositioning.h"
#include "scenariosgui.h"

#include <algorithm>
#include <codecvt>
#include <fstream>
#include <iomanip>
#include <d3d11.h>
#include <map>
#include <sstream>
#include "misc/cpp/imgui_stdlib.h"
#pragma execution_character_set("utf-8")

using namespace ScenariosEditorElementsData;
using namespace ScenariosEditorScenarioElement;
using namespace ScenariosEditorCanvasPositioning;

namespace ScenariosEditorGUI {

#pragma region Definitions

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
	ImDrawList* canvas_draw_list;

	// Helper data
	struct ElementOnCanvas
	{
		int Element = -1;
		ImVec2 Pos;
		int Type = -1;
		std::shared_ptr<ScenarioElement> ElementInStorage;
	};

	struct LinkOnCanvas
	{
		int Points[2];
		int Elems[2];
	};

	static ImVec2 scrolling(0.0f, 0.0f);
	static ImVec2 MousePosInCanvas, origin, MousePos, canvas_sz;
	static ImVec2 shift = { 0, 0 };
	static const float Space = 500;
	static int SelectedElemGUI = -1;
	static float CanvasZoom = 1;
	static int ShiftSelectionBeginElem = -1;
	static std::vector<int> CurrentSelectionElems;
	static std::vector<int> CurrentSelectionLinks;
	static bool IsLinking = false;
	static std::vector<ElementOnCanvas> Elems;
	static std::vector<LinkOnCanvas> Links;
	static std::vector<int> SelectedElems;
	static std::vector<int> SelectedLinks;
	static std::wstring CurrentFile = L"";
	static std::vector<ElementOnCanvas> CopyBuffer;
	static std::vector<LinkOnCanvas> LinksBuffer;

	static bool SearchOpen = false;
	static bool MapOpen = false;
	static bool CheckSave = false;

	void PreLoopSetup();
	void MainLoop();
	bool CopyBufferAction(std::string Operation, float Drag_Shift_x = NULL, float Drag_Shift_y = NULL);

	void DrawMenu();
	void MakeCanvas(const ImGuiViewport* viewport, ImGuiIO& io);
	void DrawCanvas(const ImGuiViewport* viewport, ImGuiIO& io);
	void DrawScenariosSection(const ImGuiViewport* viewport);
	void DrawElementsSection(const ImGuiViewport* viewport);
	void DrawParamsSection(const ImGuiViewport* viewport);
	void CanvasLogic(int hover_on, ImVec2* SelectionStartPosition, ImGuiIO& io);
	void SearchWindow();
	std::vector<int> DoSearch(int SearchType, int SearchElement, std::string SearchAttribute, std::string SearchField);
	void MapWindow();

	void ClearElements();
	void ElementsMakeObjects();
	void AddCanvasContextMenu();
	void AddDragAndDropReciever();
	std::shared_ptr<ScenarioElement> AddElementGUI(int Element, ImVec2 Pos, int Type, std::shared_ptr<ScenarioElement> CopyOrigin = nullptr);
	void AddLinkGUI(int PointA, int PointB, int ElemA, int ElemB);
	std::vector<ElementOnCanvas>::iterator DeleteElementGUI(std::vector<ElementOnCanvas>::iterator iter);
	std::vector<LinkOnCanvas>::iterator DeleteLinkGUI(std::vector<LinkOnCanvas>::iterator iter);
	std::vector<const char*> GetElementAttributeNames(ScenariosEditorScenarioElement::ScenarioElement* Elem);
	void RemoveSelectedScenario(std::string GUID);
	void DoubleSelectedScenario(std::string GUID);
	static void HelpMarker(const char* desc);
	void CanvasDrawLinking();
	void CanvasLinkingLogic(int* hover_on);
	void CanvasDrawElems();
	void CanvasDrawCaption();
	void CanvasDrawSelectedElems();
	void CanvasElemsLogic(int* hover_on, ImVec2* SelectionStartPosition);
	void CanvasLinkingSelection(int* hover_on, ImVec2* SelectionStartPosition);
	void CanvasDrawSelectedLinks();
	void CanvasSelectedElemsLogic(ImGuiIO& io);
	void AddCanvasScrollbar(int* hover_on);
	void ParamsInitialization();
	void Copy();
	void Paste();
	void Delete();
	void DeleteLinks();
	void Open();
	void New();
	void Save();
	void SaveAs();
	void SaveCopyAs();
	void SwitchSearch();
	void SwitchMap();
	void Cut();
	void OpenRecent(const wchar_t* File);
	std::vector<ImVec2> GetCorners();
	std::string toUtf8(const std::wstring& str);
	ImVec2 GetLinkingPointLocation(int Elem, int Point);
	ImVec2 GetMapLinkingPointLocation(int Elem, int Point, float Zoom, ImVec2 origin, float shift_x, float shift_y);

#pragma endregion Local variables, struct and function definitions

#pragma region Helper

	// Check if file exists
	inline bool file_exists(std::wstring path) {
		std::ifstream ff(path.c_str());
		return ff.is_open();
	}

	// Returns index of element in "Elems" vector, also used in scenariogui.cpp
	int GetNumOfElement(std::shared_ptr<ScenariosEditorScenarioElement::ScenarioElement> Elem)
	{
		for (int i = 0; i < Elems.size(); i++)
		{
			if (Elems[i].ElementInStorage == Elem)
				return i;
		}
		return -1;
	}

	// Give linking points coordinates depending on elem's type and texture
	ImVec2 GetLinkingPointLocation(int Elem, int Point)
	{
		switch (Point)
		{
		case 0: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Width / 2.0f,
			origin.y + Elems[Elem].Pos.y * CanvasZoom
		); break;
		case 1: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Width,
			origin.y + Elems[Elem].Pos.y * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Height / 2.0f
		); break;
		case 2: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Width / 2.0f,
			origin.y + Elems[Elem].Pos.y * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Height
		); break;
		case 3: return ImVec2(
			origin.x + Elems[Elem].Pos.x * CanvasZoom,
			origin.y + Elems[Elem].Pos.y * CanvasZoom + ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom).Height / 2.0f
		); break;
		}
		throw;
	}

	// Same, with overriding Zoom, origin, shift_x, shift_y
	ImVec2 GetMapLinkingPointLocation(int Elem, int Point, float Zoom, ImVec2 origin, float shift_x, float shift_y)
	{
		switch (Point)
		{
		case 0: return ImVec2(
			origin.x + (Elems[Elem].Pos.x + shift_x) * Zoom + ElementsData::GetElementTexture(Elems[Elem].Element, Zoom).Width / 2.0f,
			origin.y + (Elems[Elem].Pos.y + shift_y) * Zoom
		); break;
		case 1: return ImVec2(
			origin.x + (Elems[Elem].Pos.x + shift_x) * Zoom + ElementsData::GetElementTexture(Elems[Elem].Element, Zoom).Width,
			origin.y + (Elems[Elem].Pos.y + shift_y) * Zoom + ElementsData::GetElementTexture(Elems[Elem].Element, Zoom).Height / 2.0f
		); break;
		case 2: return ImVec2(
			origin.x + (Elems[Elem].Pos.x + shift_x) * Zoom + ElementsData::GetElementTexture(Elems[Elem].Element, Zoom).Width / 2.0f,
			origin.y + (Elems[Elem].Pos.y + shift_y) * Zoom + ElementsData::GetElementTexture(Elems[Elem].Element, Zoom).Height
		); break;
		case 3: return ImVec2(
			origin.x + (Elems[Elem].Pos.x + shift_x) * Zoom,
			origin.y + (Elems[Elem].Pos.y + shift_y) * Zoom + ElementsData::GetElementTexture(Elems[Elem].Element, Zoom).Height / 2.0f
		); break;
		}
		throw;
	}

	// Returns corners of existing elements, used for map creation, scrolling,
	std::vector<ImVec2> GetCorners()
	{
		std::vector<ImVec2> ret;
		ImVec2 Min{ FLT_MAX, FLT_MAX };
		ImVec2 Max{ -FLT_MAX, -FLT_MAX };
		for (ElementOnCanvas Elem : Elems)
		{
			if (Elem.Pos.x < Min.x) Min.x = Elem.Pos.x;
			if (Elem.Pos.y < Min.y) Min.y = Elem.Pos.y;
			if (Elem.Pos.x > Max.x) Max.x = Elem.Pos.x;
			if (Elem.Pos.y > Max.y) Max.y = Elem.Pos.y;
		}
		ret.push_back(Min);
		ret.push_back(Max);
		return ret;
	}

	float Distance(ImVec2 v, ImVec2 w, ImVec2 p) {
		// Return minimum distance between line segment vw and point p
		const float l2 = powf((w.x - v.x), 2) + powf((w.y - v.y), 2);
		if (l2 == 0.0) return sqrt(powf(p.x - v.x, 2) + powf(p.y - v.y, 2));   // v == w case
		// Consider the line extending the segment, parameterized as v + t (w - v).
		// We find projection of point p onto the line. 
		// It falls where t = [(p-v) . (w-v)] / |w-v|^2
		// We clamp t from [0,1] to handle points outside the segment vw.
		const float t = max(0, min(1, ((p.x - v.x) * (w.x - v.x) + (p.y - v.y) * (w.y - v.y)) / l2));
		const ImVec2 projection = { (w.x - v.x) * t + v.x, (w.y - v.y) * t + v.y };  // Projection falls on the segment
		return sqrt(powf(p.x - projection.x, 2) + powf(p.y - projection.y, 2));
	}

	std::string toUtf8(const std::wstring& str)
	{
		std::string ret;
		int len = WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), NULL, 0, NULL, NULL);
		if (len > 0)
		{
			ret.resize(len);
			WideCharToMultiByte(CP_UTF8, 0, str.c_str(), (int)str.length(), &ret[0], len, NULL, NULL);
		}
		return ret;
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
#pragma endregion Functions that are used for subsidiary calculations

#pragma region Window startup

	// Contains instructions should be executed before main loop
	void PreLoopSetup()
	{
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		ImGui::StyleColorsLight();

		ImVector<ImWchar> ranges;
		ImFontGlyphRangesBuilder builder;
		builder.AddChar(0x2116);                               // Some chars are loaded manually
		builder.AddChar(0x2013);
		builder.AddChar(0x202F);
		builder.AddRanges(io.Fonts->GetGlyphRangesCyrillic()); // Add one of the default ranges
		builder.BuildRanges(&ranges);                          // Build the final result (ordered ranges with all the unique characters submitted)

		io.Fonts->AddFontFromFileTTF(u8"C:/Users/VR/Desktop/projects/SimulatorsEditor/src/editor/LiberationSans.ttf", 22.0f, NULL, ranges.Data);
		io.Fonts->Build();
		ElementsData::Initialization();
		ScenarioEditorSavedSettings::Initialization();
		ScenarioEditorScenarioStorage::ClearScenarioStorage();
		SetCanvasZoomRef(&CanvasZoom);
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
			ImGui::ShowDemoWindow(); // remove later
			// Menu
			DrawMenu();
			// Window
			MakeCanvas(viewport, io);
			DrawScenariosSection(viewport);
			DrawElementsSection(viewport);
			DrawParamsSection(viewport);
			SearchWindow();
			MapWindow();

			ScenariosEditorRender::EndFrame();
		}
	}

#pragma endregion Functions initializing main loop
	
#pragma region GUI-to-Storage

	std::shared_ptr<ScenarioElement> AddElementGUI(int Element, ImVec2 Pos, int Type, std::shared_ptr<ScenarioElement> CopyOrigin)
	{
		std::shared_ptr<ScenarioElement> NewElem;
		if (ScenariosEditorElementsData::ElementsData::GetElementName(Element) == u8"Start")
		{
			for (ElementOnCanvas Elem : Elems)
				if (ScenariosEditorElementsData::ElementsData::GetElementName(Elem.Element) == u8"Start") return NULL;
			// Only one "Start" element should be allowed
		}
		unsigned int type = ElementsData::GetElementType(Element);
		int pin_count;
		for (pin_count = 0; type > 0; type >>= 1)
			if (type & 1) pin_count++;
		CopyOrigin == nullptr ?
			Elems.push_back({ Element, Pos, ElementsData::GetElementType(Element),
				NewElem = ScenariosEditorScenarioElement::AddScenarioElementStorageElement(ScenariosEditorElementsData::ElementsData::GetElementName(Element), Pos.x, Pos.y, 0.00f, pin_count) })
			:
			Elems.push_back({ Element, Pos, ElementsData::GetElementType(Element),
				NewElem = ScenariosEditorScenarioElement::AddScenarioElementStorageElement(ScenariosEditorElementsData::ElementsData::GetElementName(Element), Pos.x, Pos.y, 0.00f, pin_count, CopyOrigin) });

		CheckSave = true;
		return NewElem;
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
		CheckSave = true;
	}

	std::vector<ElementOnCanvas>::iterator DeleteElementGUI(std::vector<ElementOnCanvas>::iterator iter)
	{
		CheckSave = true;
		ScenariosEditorScenarioElement::RemoveScenarioElementStorageElement((*iter).ElementInStorage);
		return Elems.erase(iter);
	}

	std::vector<LinkOnCanvas>::iterator DeleteLinkGUI(std::vector<LinkOnCanvas>::iterator iter)
	{
		CheckSave = true;
		std::vector<int> ToRem;
		int index = ScenariosEditorScenarioElement::GetPinIndex((*Elems[(*iter).Elems[0]].ElementInStorage).getElementName().c_str(), (*iter).Points[0]);
		ToRem.push_back((*Elems[(*iter).Elems[0]].ElementInStorage).pins[index]);
		index = ScenariosEditorScenarioElement::GetPinIndex((*Elems[(*iter).Elems[1]].ElementInStorage).getElementName().c_str(), (*iter).Points[1]);
		ToRem.push_back((*Elems[(*iter).Elems[1]].ElementInStorage).pins[index]);
		ScenariosEditorScenarioElement::RemoveScenarioElementStorageLink(ToRem);
		return Links.erase(iter);
	}

#pragma endregion GUI-storage interaction functions

#pragma region Menu and File

	void DrawMenu()
	{
		ImGui::BeginMainMenuBar();
		if (ImGui::BeginMenu(u8"Файл"))
		{
			if (ImGui::MenuItem(u8"Новый", "Ctrl+N"))
			{
				New();
			}
			if (ImGui::MenuItem(u8"Открыть", "Ctrl+O"))
			{
				Open();
			}
			std::vector<std::string> RecentFiles = ScenarioEditorSavedSettings::GetRecentFiles();
			if (ImGui::BeginMenu(u8"Недавние файлы...", RecentFiles.size() > 0))
			{
				for (std::string File : RecentFiles)
				{
					std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> converter;
					std::wstring wsTmp = converter.from_bytes(File.c_str());
					if (ImGui::MenuItem(File.c_str(), (const char*)0, false, file_exists(wsTmp)))
					{
						OpenRecent(wsTmp.c_str());
					}
				}
				ImGui::EndMenu();
			}
			if (ImGui::MenuItem(u8"Сохранить", "Ctrl+S", false, CurrentFile != L""))
			{
				Save();
			}
			if (ImGui::MenuItem(u8"Сохранить как...", "Ctrl+Alt+S"))
			{
				SaveAs();
			}
			if (ImGui::MenuItem(u8"Сохранить копию как...", "Ctrl+Shift+S"))
			{
				SaveCopyAs();
			}
			ImGui::EndMenu();

		}
		if (ImGui::BeginMenu(u8"Инструменты"))
		{
			if (ImGui::MenuItem(u8"Поиск", "Ctrl+F", SearchOpen))
			{
				SwitchSearch();
			}
			if (ImGui::MenuItem(u8"Карта сценария", "Ctrl+M", MapOpen))
			{
				SwitchMap();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	void New()
	{
		if (CheckSave)
		{
			switch (MessageBox(NULL, L"Сохранить изменения?", L"Несохранённые изменения",
				MB_YESNOCANCEL | MB_DEFBUTTON3))
			{
			case IDCANCEL:
			{
				return;
			}
			case IDYES:
			{
				if (CurrentFile != L"")
				{
					Save();
				}
				else
				{
					SaveAs();
					if (CurrentFile == L"")
						return;
				}
				break;
			}
			case IDNO:
				break;
			}
		}
		ClearElements();
		ClearScenarioElementStorage();
		ScenarioEditorScenarioStorage::ClearScenarioStorage();
		SetCanvasZoomRef(&CanvasZoom);
		SetCanvasScrollingRef(&scrolling);
		SetDefaulScreenPos();
		ImGui::GetIO().ClearInputKeys();
		ScenarioEditorScenarioStorage::SetActualScenario(0);
		ScenariosEditorScenarioElement::LoadElements();
		CurrentFile = L"";
	}

	void Open()
	{
		if (CheckSave)
		{
			switch (MessageBox(NULL, L"Сохранить изменения?", L"Несохранённые изменения",
				MB_YESNOCANCEL | MB_DEFBUTTON3))
			{
			case IDCANCEL:
			{
				return;
			}
			case IDYES:
			{
				if (CurrentFile != L"")
				{
					Save();
				}
				else
				{
					SaveAs();
					if (CurrentFile == L"")
						return;
				}
				break;
			}
			case IDNO:
				break;
			}
		}
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
				CheckSave = false;
				ScenarioEditorSavedSettings::AddToRecentFiles(File);
				ScenarioEditorScenarioStorage::SetActualScenario(0);
				ScenariosEditorScenarioElement::LoadElements();
			}
			else MessageBoxW(NULL, L"При попытке открыть файл возникла ошибка", L"Ошибка", MB_OK);
		}
	}

	void OpenRecent(const wchar_t* File)
	{
		if (CheckSave)
		{
			switch (MessageBox(NULL, L"Сохранить изменения?", L"Несохранённые изменения",
				MB_YESNOCANCEL | MB_DEFBUTTON3))
			{
			case IDCANCEL:
			{
				return;
			}
			case IDYES:
			{
				if (CurrentFile != L"")
				{
					Save();
				}
				else
				{
					SaveAs();
					if (CurrentFile == L"")
						return;
				}
				break;
			}
			case IDNO:
				break;
			}
		}
		if (Model.CheckFile(File))
		{
			ScenariosEditorScenarioElement::ClearScenarioElementStorage();
			ClearElements();
			Model.LoadFrom(File);
			CurrentFile = File;
			SelectedElemGUI = -1;
			CheckSave = false;
			ScenarioEditorSavedSettings::AddToRecentFiles(File);
			ScenarioEditorScenarioStorage::SetActualScenario(0);
			ScenariosEditorScenarioElement::LoadElements();
		}
		else MessageBoxW(NULL, L"При попытке открыть файл возникла ошибка", L"Ошибка", MB_OK);
	}

	void Save()
	{
		if (CurrentFile != L"")
		{
			if (!Model.SaveTo(CurrentFile.c_str()))
			{
				MessageBoxW(NULL, L"При попытке сохранить файл возникла ошибка", L"Ошибка", MB_OK);
			}
			else CheckSave = false;
		}

	}

	void SaveAs()
	{
		const wchar_t* File = ScenarioEditorFileDialog::SaveFileDialog();
		if (size_t(File) != 0)
		{
			if (!Model.SaveTo(File))
			{
				MessageBoxW(NULL, L"При попытке сохранить файл возникла ошибка", L"Ошибка", MB_OK);
			}
			else
			{
				CurrentFile = File;
				CheckSave = false;
				ScenarioEditorSavedSettings::AddToRecentFiles(File);
			}
		}

	}

	void SaveCopyAs()
	{
		const wchar_t* File = ScenarioEditorFileDialog::SaveFileDialog();
		if (size_t(File) != 0)
		{
			if (!Model.SaveTo(File))
			{
				MessageBoxW(NULL, L"При попытке сохранить файл возникла ошибка", L"Ошибка", MB_OK);
			}
		}
	}

#pragma endregion Menu and file interaction functions

#pragma region Canvas
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




		CanvasElemsLogic(&hover_on, &SelectionStartPosition);



		CanvasLinkingSelection(&hover_on, &SelectionStartPosition);
		CanvasSelectedElemsLogic(io);

		CanvasLinkingLogic(&hover_on);

		CanvasDrawCaption();
		CanvasDrawLinking();
		CanvasDrawSelectedLinks();
		CanvasDrawElems();
		CanvasDrawSelectedElems();
		AddCanvasScrollbar(&hover_on);

		CanvasLogic(hover_on, &SelectionStartPosition, io);

		ImGui::End();
	}

	void CanvasDrawSelectedLinks()
	{
		for (int Link : SelectedLinks)
		{
			canvas_draw_list->AddLine(
				GetLinkingPointLocation(Links[Link].Elems[0], Links[Link].Points[0]),
				GetLinkingPointLocation(Links[Link].Elems[1], Links[Link].Points[1]),
				IM_COL32(255, 0, 0, 255), 3
			);
		}
	}

	void CanvasLinkingSelection(int* hover_on, ImVec2* selection_start_position)
	{
		if (CurrentState == Rest && (*hover_on) == 1 && (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right)))
		{
			for (int i = 0; i < Links.size(); i++)
			{
				ImVec2 a = GetLinkingPointLocation(Links[i].Elems[0], Links[i].Points[0]);
				ImVec2 b = GetLinkingPointLocation(Links[i].Elems[1], Links[i].Points[1]);
				if (Distance(a, b, MousePos) < 3)
				{
					if (ImGui::GetIO().KeyCtrl)
					{
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							std::vector<int>::iterator t = std::find(SelectedLinks.begin(), SelectedLinks.end(), i);
							if (t == SelectedLinks.end())
							{
								SelectedLinks.push_back(i);
							}
							else
							{
								SelectedLinks.erase(t);
							}
						}
					}
					else
					{
						if (std::find(SelectedLinks.begin(), SelectedLinks.end(), i) == SelectedLinks.end())
						{
							SelectedElems.clear();
							SelectedLinks.clear();
							SelectedLinks.push_back(i);
						}
					}
					*hover_on = 5;
				}

			}
		}
		else if (CurrentState == Selection)
		{
			for (int i = 0; i < Links.size(); i++)
			{
				ImVec2 a = GetLinkingPointLocation(Links[i].Elems[0], Links[i].Points[0]);
				ImVec2 b = GetLinkingPointLocation(Links[i].Elems[1], Links[i].Points[1]);
				ImVec2 Mid{ ((a.x + b.x) / 2 - origin.x) / CanvasZoom , ((a.y + b.y) / 2 - origin.y) / CanvasZoom };
				if (Mid.x > min(selection_start_position->x, MousePosInCanvas.x)
					&& Mid.x < max(selection_start_position->x, MousePosInCanvas.x)
					&& Mid.y > min(selection_start_position->y, MousePosInCanvas.y)
					&& Mid.y < max(selection_start_position->y, MousePosInCanvas.y)
					)
				{
					if (!ImGui::GetIO().KeyCtrl)
					{
						if (std::find(SelectedLinks.begin(), SelectedLinks.end(), i) == SelectedLinks.end())
							SelectedLinks.push_back(i);
					}
					else
					{
						std::vector<int>::iterator t = std::find(SelectedLinks.begin(), SelectedLinks.end(), i);
						if (t == SelectedLinks.end())
						{
							SelectedLinks.push_back(i);
						}
						else
						{
							SelectedLinks.erase(t);
						}

					}
					CurrentSelectionLinks.push_back(i);
				}
			}
		}
		else
		{
			if (!ImGui::IsAnyItemActive() && ImGui::GetIO().KeyCtrl && ImGui::IsKeyPressed(ImGuiKey_A, false))
			{
				for (int i = 0; i < Links.size(); i++)
				{
					if (std::find(SelectedLinks.begin(), SelectedLinks.end(), i) == SelectedLinks.end())
					{
						SelectedLinks.push_back(i);
					}
				}
			}
		}
	}

	void AddCanvasScrollbar(int* hover_on)
	{
		static const int short_side = 25;
		static const int offset = 0;
		static const ImVec2 inner_spacing{ 4, 2 }; // x = from long side, y = From short side
		ImVec2 OldPos = ImGui::GetCursorPos();

		float scroll_y_max = canvas_sz.y - offset * 3 - short_side - inner_spacing.y * 2 - 1;
		float scroll_x_max = canvas_sz.x - offset * 3 - short_side - inner_spacing.y * 2 - 1;

		std::vector<ImVec2> Corners = ScenariosEditorGUI::GetCorners();
		// 0 - Min
		// 1 - Max

		ImVec2 ScreenTopLeft = { -scrolling.x - shift.x / CanvasZoom, -scrolling.y - shift.y / CanvasZoom };
		if (ScreenTopLeft.y + Space < Corners[0].y) Corners[0].y = ScreenTopLeft.y + Space;
		if (ScreenTopLeft.x + Space < Corners[0].x) Corners[0].x = ScreenTopLeft.x + Space;

		ImVec2 ScreenBottomRight = { ScreenTopLeft.x + canvas_sz.x / CanvasZoom, ScreenTopLeft.y + canvas_sz.y / CanvasZoom };
		if (ScreenBottomRight.y - Space > Corners[1].y) Corners[1].y = ScreenBottomRight.y - Space;
		if (ScreenBottomRight.x - Space > Corners[1].x) Corners[1].x = ScreenBottomRight.x - Space;

		float ActualRangeY = Corners[1].y - Corners[0].y + 2 * Space;
		float ActualRangeX = Corners[1].x - Corners[0].x + 2 * Space;
		float ViewRangeY = canvas_sz.y / (CanvasZoom);
		float ViewRangeX = canvas_sz.x / (CanvasZoom);

		float scroll_y_begin = (ScreenTopLeft.y - Corners[0].y + Space) * scroll_y_max / (Corners[1].y - Corners[0].y + 2 * Space);
		float scroll_x_begin = (ScreenTopLeft.x - Corners[0].x + Space) * scroll_x_max / (Corners[1].x - Corners[0].x + 2 * Space);
		float scroll_y_height = ViewRangeY * scroll_y_max / ActualRangeY;
		float scroll_x_widht = ViewRangeX * scroll_x_max / ActualRangeX;

		bool scroll_y_enable = scroll_y_height + 1 < scroll_y_max;
		bool scroll_x_enable = scroll_x_widht + 1 < scroll_x_max;
		static bool scrolling_y = false;
		static bool scrolling_x = false;

		if (scroll_y_enable)
		{
			static ImU32 scroll_color = scrolling_y ? IM_COL32(128, 128, 128, 255) : IM_COL32(192, 192, 192, 255);
			canvas_draw_list->AddRectFilled({ ImGui::GetWindowPos().x + 1 + canvas_sz.x - offset - short_side, ImGui::GetWindowPos().y + 1 + offset },
				{ ImGui::GetWindowPos().x + canvas_sz.x - offset , ImGui::GetWindowPos().y + 1 + canvas_sz.y - offset * 2 - short_side },
				IM_COL32(255, 255, 255, 255));

			canvas_draw_list->AddRectFilled({ ImGui::GetWindowPos().x + 1 + canvas_sz.x - offset - short_side + inner_spacing.x, ImGui::GetWindowPos().y + 1 + offset + inner_spacing.y + scroll_y_begin },
				{ ImGui::GetWindowPos().x + canvas_sz.x - offset - inner_spacing.x , ImGui::GetWindowPos().y + 1 + offset + inner_spacing.y + scroll_y_begin + scroll_y_height },
				scroll_color);

			ImGui::SetCursorPos({ canvas_sz.x - offset - short_side, offset });

			ImGui::InvisibleButton(u8"##scrollbar_y", { short_side , canvas_sz.y - offset * 3 - short_side + 2 });

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				scrolling_y = true;
			}
			else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				scrolling_y = false;
			}
			if (scrolling_y)
			{
				*hover_on = 4;
				float MouseY = (ImGui::GetIO().MousePos.y - ImGui::GetWindowPos().y - 1 - offset - inner_spacing.y) - scroll_y_height / 2;
				scrolling.y = min(-(shift.y / CanvasZoom + Corners[0].y - Space + (ActualRangeY * MouseY / scroll_y_max)), -(shift.y / CanvasZoom + Corners[0].y - Space));
				scrolling.y = max(scrolling.y, -(shift.y / CanvasZoom + Corners[1].y + Space - canvas_sz.y / CanvasZoom));
			}
		}
		else
		{
			scrolling_y = false;
		}

		if (scroll_x_enable)
		{
			static ImU32 scroll_color = scrolling_x ? IM_COL32(128, 128, 128, 255) : IM_COL32(192, 192, 192, 255);
			canvas_draw_list->AddRectFilled({ ImGui::GetWindowPos().x + 1 + offset, ImGui::GetWindowPos().y + 1 + canvas_sz.y - offset - short_side },
				{ ImGui::GetWindowPos().x + 1 + canvas_sz.x - offset * 2 - short_side, ImGui::GetWindowPos().y + canvas_sz.y - offset },
				IM_COL32(255, 255, 255, 255));

			canvas_draw_list->AddRectFilled({ ImGui::GetWindowPos().x + 1 + offset + inner_spacing.y + scroll_x_begin , ImGui::GetWindowPos().y + 1 + canvas_sz.y - offset - short_side + inner_spacing.x },
				{ ImGui::GetWindowPos().x + offset + inner_spacing.y + scroll_x_begin + scroll_x_widht , ImGui::GetWindowPos().y + 1 + canvas_sz.y - offset - inner_spacing.x },
				scroll_color);

			ImGui::SetCursorPos({ offset, canvas_sz.y - offset - short_side });

			ImGui::InvisibleButton(u8"##scrollbar_x", { canvas_sz.x - offset * 3 - short_side + 2, short_side });

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				scrolling_x = true;
			}
			else if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				scrolling_x = false;
			}
			if (scrolling_x)
			{
				*hover_on = 4;
				float MouseX = (ImGui::GetIO().MousePos.x - ImGui::GetWindowPos().x - 1 - offset - inner_spacing.y) - scroll_x_widht / 2;
				scrolling.x = min(-(shift.x / CanvasZoom + Corners[0].x - Space + (ActualRangeX * MouseX / scroll_x_max)), -(shift.x / CanvasZoom + Corners[0].x - Space));
				scrolling.x = max(scrolling.x, -(shift.x / CanvasZoom + Corners[1].x + Space - canvas_sz.x / CanvasZoom));
			}
		}
		else
		{
			scrolling_x = false;
		}

		if (offset == 0 && scroll_y_enable && scroll_x_enable)
		{
			canvas_draw_list->AddRectFilled({ ImGui::GetWindowPos().x + 1 + canvas_sz.x - short_side, ImGui::GetWindowPos().y + 1 + canvas_sz.y - short_side },
				{ ImGui::GetWindowPos().x + 1 + canvas_sz.x,ImGui::GetWindowPos().y + 1 + canvas_sz.y },
				IM_COL32(255, 255, 255, 255));
		}

		ImGui::SetCursorPos(OldPos);
	}
	// Clear existing elements
	void ClearElements()
	{
		IsLinking = false;
		SelectedLinks.clear();
		SelectedElems.clear();
		Elems.clear();
		Links.clear();
		CurrentState = Rest;
		ShiftSelectionBeginElem = -1;
		CopyBufferAction("Clear");
	}

	void AddDragAndDropReciever()
	{
		// canvas is drag'n'drop reciever
		if (ImGui::BeginDragDropTarget()) {

			auto payload = ImGui::AcceptDragDropPayload("Element");
			if (payload != NULL) {
				int ElementNum = *(int*)payload->Data;
				float x = MousePosInCanvas.x - (ElementsData::GetElementTexture(ElementNum, CanvasZoom).Width / 2.0f) / CanvasZoom;
				float y = MousePosInCanvas.y - (ElementsData::GetElementTexture(ElementNum, CanvasZoom).Height / 2.0f) / CanvasZoom;
				if (AddElementGUI(ElementNum, ImVec2(x, y), ElementsData::GetElementType(ElementNum)))
				{
					CopyBufferAction("Elem_Add");
					SelectedElems.clear();
					SelectedElems.push_back((int)Elems.size() - 1);
				}
			}
			ImGui::EndDragDropTarget();
		}
	}

	// Context menu used for canvas
	void AddCanvasContextMenu() // TODO: split function
	{
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		if (abs(drag_delta.x) <= 1.0f && abs(drag_delta.y <= 1.0f))
		{
			ImGui::OpenPopupOnItemClick("contextworkspace", ImGuiPopupFlags_MouseButtonRight);
			if (ImGui::BeginPopup("contextworkspace"))
			{
				if (ImGui::MenuItem(u8"Копировать", u8"Ctrl+C", false, SelectedElems.size() > 0))
				{
					Copy();
				}
				if (ImGui::MenuItem(u8"Вырезать", u8"Ctrl+X", false, SelectedElems.size() > 0))
				{
					Cut();
				}
				if (ImGui::MenuItem(u8"Вставить", u8"Ctrl+V", false, CopyBuffer.size() > 0))
				{
					Paste();
				}
				if (ImGui::MenuItem(u8"Удалить", u8"Ctrl+Delete", false, SelectedElems.size() > 0 || SelectedLinks.size() > 0))
				{
					Delete();
				}
				if (ImGui::MenuItem(u8"Удалить связи элементов", u8"Ctrl+Shift+Delete", false, SelectedElems.size() > 0))
				{
					DeleteLinks();
				}
				ImGui::Separator();
				if (ImGui::MenuItem(u8"Отменить", u8"Ctrl+Z", false, CopyBufferAction("Can_Undo")))
				{
					CopyBufferAction("Undo");
				}
				if (ImGui::MenuItem(u8"Повторить", u8"Ctrl+Y", false, CopyBufferAction("Can_Redo")))
				{
					CopyBufferAction("Redo");
				}
				ImGui::EndPopup();
			}
		}
	}

	// Add buttons which represent linking points
	void AddLinkingPoint(int* hover_on, int Point, int* ClickedElem, int* ClickedType, int Elem)
	{
		// out of state machine for logic purposes

		canvas_draw_list->AddCircleFilled(GetLinkingPointLocation(Elem, Point), 5.0f, IM_COL32(0, 0, 0, 255));
		ImGui::SetCursorScreenPos(ImVec2(GetLinkingPointLocation(Elem, Point).x - 5.0f, GetLinkingPointLocation(Elem, Point).y - 5.0f));
		ImGui::InvisibleButton("LinkingPoint", ImVec2(10.0f, 10.0f), ImGuiButtonFlags_MouseButtonLeft);
		ImGui::SetItemAllowOverlap();
		if (*hover_on == 1 && ImGui::IsItemClicked(ImGuiMouseButton_Left))
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
					CopyBufferAction("Link_Add");
				}
			}
			IsLinking = !IsLinking;
		}

		if (IsLinking)
		{
			SelectedLinks.clear();
			SelectedElems.clear();
			canvas_draw_list->AddLine(GetLinkingPointLocation(*ClickedElem, *ClickedType), MousePos, IM_COL32(0, 0, 0, 255));
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				IsLinking = false;
				ImGui::GetIO().ClearInputKeys();
			}
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
		canvas_sz = ImGui::GetContentRegionAvail();
		static ImVec2 old_canvas_sz = canvas_sz;
		SetSize(canvas_sz);

		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		MousePos = io.MousePos;
		canvas_draw_list = ImGui::GetWindowDrawList();
		canvas_draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(240, 240, 240, 0));
		// Window itself doesn't trigger io mouse actions, invisible button does
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
		ImGui::SetItemAllowOverlap();
		if (ImGui::IsItemHovered())
		{
			static const float ScrollStep = 200;
			if (ImGui::GetIO().MouseWheel != 0)
			{
				if (ImGui::GetIO().KeyCtrl)
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
				}
				else
				{
					if (ImGui::GetIO().KeyShift)
					{
						scrolling.x += ImGui::GetIO().MouseWheel * ScrollStep / CanvasZoom;
					}
					else
					{
						scrolling.y += ImGui::GetIO().MouseWheel * ScrollStep / CanvasZoom;
					}
				}
			}
		}
		if (canvas_sz.x != old_canvas_sz.x || canvas_sz.y != old_canvas_sz.y)
			shift = { -(canvas_sz.x * CanvasZoom - canvas_sz.x) / 2.0f, -(canvas_sz.y * CanvasZoom - canvas_sz.y) / 2.0f };
		canvas_draw_list->PopClipRect();
		origin = ImVec2(canvas_p0.x + scrolling.x * CanvasZoom + shift.x, canvas_p0.y + scrolling.y * CanvasZoom + shift.y);
		// Draw grid
		canvas_draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		{
			const float GRID_STEP = 79.0f * CanvasZoom;
			for (float x = fmodf(scrolling.x * CanvasZoom + shift.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
				canvas_draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 119));
			for (float y = fmodf(scrolling.y * CanvasZoom + shift.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
				canvas_draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 119));
		}
		MousePosInCanvas = ImVec2((io.MousePos.x - origin.x) / CanvasZoom, (io.MousePos.y - origin.y) / CanvasZoom);
		MousePos = io.MousePos;
		old_canvas_sz = canvas_sz;
	}
	// Draw elems
	void CanvasDrawElems()
	{
		for (int i = 0; i < Elems.size(); i++)
		{
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element, CanvasZoom);
			canvas_draw_list->AddImage(
				(void*)UsedTexture.Payload, ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom, origin.y + Elems[i].Pos.y * CanvasZoom),
				ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom + UsedTexture.Width, origin.y + Elems[i].Pos.y * CanvasZoom + UsedTexture.Height));
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
		float shift_x = 200 * CanvasZoom;
		float shift_y = 20 * CanvasZoom;
		for (ElementOnCanvas Elem : Elems)
		{
			if (origin.y + Elem.Pos.y * CanvasZoom > 0)
				canvas_draw_list->AddText(ImGui::GetFont(), font_size, ImVec2(origin.x + Elem.Pos.x * CanvasZoom + shift_x, origin.y + Elem.Pos.y * CanvasZoom + shift_y), IM_COL32(0, 0, 0, 255), (Elem.ElementInStorage)->caption.c_str(), (const char*)0, wrap_size);
		}
	}
	// Logic functions, using state machine, used to handle io actions
	//
	// Changes state machine, handles canvas-related actions
	void CanvasLogic(int clicked_on, ImVec2* SelectionStartPosition, ImGuiIO& io)
	{
		// clicked_on: 0 - offcanvas, 1 - canvas, 2 - linking point, 3 - element
		bool ShorcutsChecked = false;
		if (!ImGui::IsAnyItemActive() && !ShorcutsChecked)
		{
			ShorcutsChecked = true;
			if (ImGui::GetIO().KeyCtrl)
			{
				if (ImGui::IsKeyPressed(ImGuiKey_X, false))
				{
					Cut();
				}
				if (CurrentState == Rest && !IsLinking)
				{
					if (ImGui::IsKeyPressed(ImGuiKey_Y, false))
					{
						CopyBufferAction("Redo");
					}
					if (ImGui::IsKeyPressed(ImGuiKey_Z, false))
					{
						CopyBufferAction("Undo");
					}
				}
				if (ImGui::IsKeyPressed(ImGuiKey_N, false))
				{
					New();
					ImGui::GetIO().ClearInputKeys();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_C, false))
				{
					Copy();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_V, false) && clicked_on > 0)
				{
					Paste();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_Delete, false))
				{
					if (ImGui::GetIO().KeyShift)
					{
						DeleteLinks();
					}
					else
					{
						Delete();
					}
				}
				if (ImGui::IsKeyPressed(ImGuiKey_O))
				{
					Open();
					ImGui::GetIO().ClearInputKeys();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_S, false))
				{
					if (ImGui::GetIO().KeyAlt)
					{
						SaveAs();
						ImGui::GetIO().ClearInputKeys();
					}
					else if (ImGui::GetIO().KeyShift)
					{
						SaveCopyAs();
						ImGui::GetIO().ClearInputKeys();
					}
					else
					{
						Save();
						ImGui::GetIO().ClearInputKeys();
					}
				}
				if (ImGui::IsKeyPressed(ImGuiKey_M, false))
				{
					SwitchMap();
				}
				if (ImGui::IsKeyPressed(ImGuiKey_F, false))
				{
					SwitchSearch();
				}
			}
		}
		if (CurrentState == Selection && ImGui::IsMouseReleased(ImGuiMouseButton_Left))
		{
			if (CurrentSelectionElems.size() != 0)
			{
				int MinimalElement = CurrentSelectionElems[0];
				for (int i = 1; i < CurrentSelectionElems.size(); i++)
				{
					if (Elems[CurrentSelectionElems[i]].Pos.y < Elems[MinimalElement].Pos.y)
						MinimalElement = CurrentSelectionElems[i];
				}
				ShiftSelectionBeginElem = MinimalElement;
				CurrentSelectionElems.clear();
			}
			CurrentSelectionLinks.clear();
		}
		// Right-mouse dragging causes canvas dragging
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right) && CurrentState == Rest && clicked_on == 1)
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
			*SelectionStartPosition = MousePosInCanvas;
			if (!ImGui::GetIO().KeyShift && !ImGui::GetIO().KeyCtrl)
			{
				SelectedLinks.clear();
				SelectedElems.clear();
			}
		}
		static float Drag_Shift_x = 0;
		static float Drag_Shift_y = 0;
		if (CurrentState == ElementDragging)
		{
			Drag_Shift_x += io.MouseDelta.x / CanvasZoom;
			Drag_Shift_y += io.MouseDelta.y / CanvasZoom;
		}
		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && clicked_on == 3 && CurrentState == Rest)
		{
			CurrentState = ElementDragging;
		}
		if (ImGui::IsMouseReleased(ImGuiMouseButton_Left) && clicked_on != 2)
		{
			if (CurrentState == ElementDragging)
			{
				CopyBufferAction("Elem_Group_Move", Drag_Shift_x, Drag_Shift_y);
				Drag_Shift_x = 0;
				Drag_Shift_y = 0;
			}
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
			canvas_draw_list->AddRect({ origin.x + SelectionStartPosition->x * CanvasZoom, origin.y + SelectionStartPosition->y * CanvasZoom }, io.MousePos, IM_COL32(0, 0, 0, 255));
			canvas_draw_list->AddRectFilled({ origin.x + SelectionStartPosition->x * CanvasZoom, origin.y + SelectionStartPosition->y * CanvasZoom }, io.MousePos, IM_COL32(0, 0, 0, 15));
			if (!ImGui::GetIO().KeyShift && !ImGui::GetIO().KeyCtrl)
			{
				SelectedLinks.clear();
				SelectedElems.clear();
			}
			else
			{
				for (int i = 0; i < CurrentSelectionElems.size(); i++)
				{
					std::vector<int>::iterator t = std::find(SelectedElems.begin(), SelectedElems.end(), CurrentSelectionElems[i]);
					if (t != SelectedElems.end())
					{
						SelectedElems.erase(t);
					}
					else if (ImGui::GetIO().KeyCtrl)
					{
						SelectedElems.push_back(CurrentSelectionElems[i]);
					}
				}
				for (int i = 0; i < CurrentSelectionLinks.size(); i++)
				{
					std::vector<int>::iterator t = std::find(SelectedLinks.begin(), SelectedLinks.end(), CurrentSelectionLinks[i]);
					if (t != SelectedLinks.end())
					{
						SelectedLinks.erase(t);
					}
					else if (ImGui::GetIO().KeyCtrl)
					{
						SelectedLinks.push_back(CurrentSelectionLinks[i]);
					}
				}
			}
			CurrentSelectionElems.clear();
			CurrentSelectionLinks.clear();
			break;
		}
	}

	// Handles elems-depending logic
	void CanvasElemsLogic(int* hover_on, ImVec2* SelectionStartPosition)
	{
		if (!ImGui::IsAnyItemActive() && ImGui::GetIO().KeyShift)
		{
			if (ImGui::IsKeyPressed(ImGuiKey_Home, false))
			{
				if (!ImGui::GetIO().KeyCtrl)
				{
					SelectedElems.clear();
					SelectedLinks.clear();
				}
				if (ShiftSelectionBeginElem == -1)
				{
					if (Elems.size() > 0)
					{
						float min_y = FLT_MAX;
						int min_elem;
						for (int i = 0; i < Elems.size(); i++)
						{
							if (Elems[i].Pos.y < min_y)
							{
								min_y = Elems[i].Pos.y;
								min_elem = i;
							}
						}
						if (std::find(SelectedElems.begin(), SelectedElems.end(), min_elem) == SelectedElems.end())
						{
							SelectedElems.push_back(min_elem);
						}
					}
				}
				else
				{
					for (int i = 0; i < Elems.size(); i++)
					{
						if (Elems[i].Pos.y <= Elems[ShiftSelectionBeginElem].Pos.y && std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
						{
							SelectedElems.push_back(i);
						}
					}
					for (int k = 0; k < Links.size(); k++)
					{
						std::vector<int>::iterator el1, el2;
						if ((el1 = std::find(SelectedElems.begin(), SelectedElems.end(), Links[k].Elems[0])) != SelectedElems.end()
							&& (el2 = std::find(SelectedElems.begin(), SelectedElems.end(), Links[k].Elems[1])) != SelectedElems.end()
							&& std::find(SelectedLinks.begin(), SelectedLinks.end(), k) == SelectedLinks.end()
							)
						{
							if (Elems[*el1].Pos.y <= Elems[ShiftSelectionBeginElem].Pos.y && Elems[*el2].Pos.y <= Elems[ShiftSelectionBeginElem].Pos.y)
							{
								SelectedLinks.push_back(k);
							}
						}
					}
				}
			}
			if (ImGui::IsKeyPressed(ImGuiKey_End, false))
			{
				if (!ImGui::GetIO().KeyCtrl)
				{
					SelectedElems.clear();
					SelectedLinks.clear();
				}
				if (ShiftSelectionBeginElem == -1)
				{
					for (int i = 0; i < Elems.size(); i++)
					{
						if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
						{
							SelectedElems.push_back(i);
						}
					}
				}
				else
				{
					for (int i = 0; i < Elems.size(); i++)
					{
						if (Elems[i].Pos.y >= Elems[ShiftSelectionBeginElem].Pos.y && std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
						{
							SelectedElems.push_back(i);
						}
					}
					for (int k = 0; k < Links.size(); k++)
					{
						std::vector<int>::iterator el1, el2;
						if ((el1 = std::find(SelectedElems.begin(), SelectedElems.end(), Links[k].Elems[0])) != SelectedElems.end()
							&& (el2 = std::find(SelectedElems.begin(), SelectedElems.end(), Links[k].Elems[1])) != SelectedElems.end()
							&& std::find(SelectedLinks.begin(), SelectedLinks.end(), k) == SelectedLinks.end()
							)
						{
							if (Elems[*el1].Pos.y >= Elems[ShiftSelectionBeginElem].Pos.y && Elems[*el2].Pos.y >= Elems[ShiftSelectionBeginElem].Pos.y)
							{
								SelectedLinks.push_back(k);
							}
						}
					}
				}
			}

			/*float min_y, max_y;
			if (ShiftSelectionBeginElem != -1)
			{
				min_y = min(Elems[i].Pos.y, Elems[ShiftSelectionBeginElem].Pos.y);
				max_y = max(Elems[i].Pos.y, Elems[ShiftSelectionBeginElem].Pos.y);
			}
			else
			{
				min_y = FLT_MIN;
				max_y = Elems[i].Pos.y;
			}*/
		}
		for (int i = 0; i < Elems.size(); i++)
		{
			if (!ImGui::IsAnyItemActive() && ImGui::GetIO().KeyCtrl && std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
			{
				if (ImGui::IsKeyPressed(ImGuiKey_A, false))
				{
					SelectedElems.push_back(i);
				}

			}
			// Create button and handle it io actions
			Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element, CanvasZoom);
			ImGui::SetCursorScreenPos(ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom, origin.y + Elems[i].Pos.y * CanvasZoom));
			ImGui::InvisibleButton("canvas123", ImVec2((float)UsedTexture.Width, (float)UsedTexture.Height), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			ImGui::SetItemAllowOverlap();
			// IsMouseClicked() + IsItemHovered() usage to find which element user selected

			if (CurrentState == Rest && ImGui::IsItemHovered())
			{
				if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
				{
					Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element, CanvasZoom);
					// rectangle displaying selection
					canvas_draw_list->AddRect(
						ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom, origin.y + Elems[i].Pos.y * CanvasZoom),
						ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom + UsedTexture.Width, origin.y + Elems[i].Pos.y * CanvasZoom + UsedTexture.Height),
						IM_COL32(0, 0, 0, 128));
					canvas_draw_list->AddRectFilled(
						ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom, origin.y + Elems[i].Pos.y * CanvasZoom),
						ImVec2(origin.x + Elems[i].Pos.x * CanvasZoom + UsedTexture.Width, origin.y + Elems[i].Pos.y * CanvasZoom + UsedTexture.Height),
						IM_COL32(0, 0, 128, 40));
				}
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) || ImGui::IsMouseClicked(ImGuiMouseButton_Right))
				{
					if (ImGui::GetIO().KeyCtrl && !ImGui::GetIO().KeyShift)
					{
						if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
						{
							ShiftSelectionBeginElem = i;
							std::vector<int>::iterator t = std::find(SelectedElems.begin(), SelectedElems.end(), i);
							if (t == SelectedElems.end())
							{
								SelectedElems.push_back(i);
							}
							else
							{
								SelectedElems.erase(t);
							}
						}
					}
					else if (ImGui::GetIO().KeyShift)
					{
						if (!ImGui::GetIO().KeyCtrl || SelectedElems.size() > 0)
						{
							float min_y, max_y;
							if (ShiftSelectionBeginElem != -1)
							{
								min_y = min(Elems[i].Pos.y, Elems[ShiftSelectionBeginElem].Pos.y);
								max_y = max(Elems[i].Pos.y, Elems[ShiftSelectionBeginElem].Pos.y);
							}
							else
							{
								min_y = -FLT_MAX;
								max_y = Elems[i].Pos.y;
							}
							if (!ImGui::GetIO().KeyCtrl)
							{
								SelectedElems.clear();
								SelectedLinks.clear();
							}
							for (int j = 0; j < Elems.size(); j++)
							{
								if (Elems[j].Pos.y <= max_y && Elems[j].Pos.y >= min_y && std::find(SelectedElems.begin(), SelectedElems.end(), j) == SelectedElems.end())
								{
									SelectedElems.push_back(j);
								}
							}
							for (int k = 0; k < Links.size(); k++)
							{
								std::vector<int>::iterator el1, el2;
								if ((el1 = std::find(SelectedElems.begin(), SelectedElems.end(), Links[k].Elems[0])) != SelectedElems.end()
									&& (el2 = std::find(SelectedElems.begin(), SelectedElems.end(), Links[k].Elems[1])) != SelectedElems.end()
									&& std::find(SelectedLinks.begin(), SelectedLinks.end(), k) == SelectedLinks.end()
									)
								{
									if (Elems[*el1].Pos.y <= max_y && Elems[*el1].Pos.y >= min_y && Elems[*el2].Pos.y <= max_y && Elems[*el2].Pos.y >= min_y)
									{
										SelectedLinks.push_back(k);
									}
								}
							}
						}
					}
					else
					{
						if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
						{
							SelectedElems.clear();
							SelectedLinks.clear();
							SelectedElems.push_back(i);
							ShiftSelectionBeginElem = i;
						}
					}
				}
				*hover_on = 3;
			}
			// Foreach element we determine if it contains in selection
			if (CurrentState == Selection)
			{
				if (Elems[i].Pos.x + UsedTexture.Width / 2.0f / CanvasZoom >= min(MousePosInCanvas.x, SelectionStartPosition->x)
					&& Elems[i].Pos.x + UsedTexture.Width / 2.0f / CanvasZoom <= max(MousePosInCanvas.x, SelectionStartPosition->x)
					&& Elems[i].Pos.y + UsedTexture.Height / 2.0f / CanvasZoom >= min(MousePosInCanvas.y, SelectionStartPosition->y)
					&& Elems[i].Pos.y + UsedTexture.Height / 2.0f / CanvasZoom <= max(MousePosInCanvas.y, SelectionStartPosition->y)
					)
				{
					if (!ImGui::GetIO().KeyCtrl)
					{
						if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
							SelectedElems.push_back(i);
					}
					else
					{
						std::vector<int>::iterator t = std::find(SelectedElems.begin(), SelectedElems.end(), i);
						if (t == SelectedElems.end())
						{
							SelectedElems.push_back(i);
						}
						else
						{
							SelectedElems.erase(t);
						}

					}
					CurrentSelectionElems.push_back(i);
				}
			}
		}
	}
	//
	void CanvasSelectedElemsLogic(ImGuiIO& io)
	{
		ElementOnCanvas ToAdd;
		// dragging, clicking on elements realization
		int j = (int)SelectedElems.size();
		static bool TriggerDrag = false;

		for (int i = 0; i < j; i++)
		{
			int SelectedElem = SelectedElems[i];
			// dragging wont change order
			if (CurrentState == ElementDragging)
			{
				TriggerDrag = true;
				float x = Elems[SelectedElem].Pos.x + io.MouseDelta.x / CanvasZoom;
				float y = Elems[SelectedElem].Pos.y + io.MouseDelta.y / CanvasZoom;
				Elems[SelectedElem].Pos = ImVec2(x, y);
				ScenariosEditorScenarioElement::UpdateCoordinates(Elems[SelectedElem].ElementInStorage, x, y);
				CheckSave = true;
			}
			else if (TriggerDrag)
			{
				TriggerDrag = false;
			}
			// when SINGLE element clicked it draws on top of the others
			else if (CurrentState == Rest && j < 2)
			{
				if (ShiftSelectionBeginElem == SelectedElem)
				{
					ShiftSelectionBeginElem = (int)Elems.size() - 1;
				}
				ToAdd = { Elems[SelectedElem].Element, Elems[SelectedElem].Pos, Elems[SelectedElem].Type, Elems[SelectedElem].ElementInStorage };
				CopyBufferAction("Update");
				Elems.erase(Elems.begin() + SelectedElem);
				Elems.push_back(ToAdd);
				SelectedElems[i] = -1;
				SelectedElems.push_back((int)Elems.size() - 1 - i);
				for (int k = 0; k < Links.size(); k++)
				{
					if (Links[k].Elems[0] == SelectedElem) Links[k].Elems[0] = (int)Elems.size() - 1;
					else if (Links[k].Elems[0] > SelectedElem) Links[k].Elems[0]--;
					if (Links[k].Elems[1] == SelectedElem) Links[k].Elems[1] = (int)Elems.size() - 1;
					else if (Links[k].Elems[1] > SelectedElem) Links[k].Elems[1]--;
				}
			}
		}
		auto new_end = std::remove(SelectedElems.begin(), SelectedElems.end(), -1);
		SelectedElems.erase(new_end, SelectedElems.end());
	}
	//
	void CanvasLinkingLogic(int* hover_on)
	{
		static int ClickedElem;
		static int ClickedType;
		for (int i = 0; i < Elems.size(); i++)
		{
			if (Elems[i].Type & 1)
			{
				AddLinkingPoint(hover_on, 0, &ClickedElem, &ClickedType, i);
			}
			if (Elems[i].Type & 2)
			{
				AddLinkingPoint(hover_on, 1, &ClickedElem, &ClickedType, i);
			}
			if (Elems[i].Type & 4)
			{
				AddLinkingPoint(hover_on, 2, &ClickedElem, &ClickedType, i);
			}
			if (Elems[i].Type & 8)
			{
				AddLinkingPoint(hover_on, 3, &ClickedElem, &ClickedType, i);
			}
		}
	}

#pragma endregion Canvas window related functions

#pragma region Elements operations

	void Cut()
	{
		Copy();
		Delete();
	}

	void Copy()
	{
		if (SelectedElems.size() > 0)
		{
			CopyBuffer.clear();
			LinksBuffer.clear();
			std::vector<int> Positions;
			for (int i = 0; i < SelectedElems.size(); i++)
			{
				CopyBuffer.push_back(Elems[SelectedElems[i]]);
				Positions.push_back(SelectedElems[i]);
			}
			for (int k = 0; k < SelectedLinks.size(); k++)
			{
				int Elem1 = (int)(find(Positions.begin(), Positions.end(), Links[SelectedLinks[k]].Elems[0]) - Positions.begin());
				int Elem2 = (int)(find(Positions.begin(), Positions.end(), Links[SelectedLinks[k]].Elems[1]) - Positions.begin());
				if (Elem1 < Positions.size() && Elem2 < Positions.size())
					LinksBuffer.push_back({ {Links[SelectedLinks[k]].Points[0], Links[SelectedLinks[k]].Points[1]},{Elem1, Elem2} });
			}
		}
	}

	void Paste()
	{
		if (CopyBuffer.size() > 0)
		{
			SelectedElems.clear();
			SelectedLinks.clear();
			ImVec2 min = ImVec2(FLT_MAX, FLT_MAX);
			int ElemsSize = (int)Elems.size();
			std::map<int, int> ToChange;
			for (int i = 0; i < CopyBuffer.size(); i++)
			{
				if (CopyBuffer[i].Pos.x < min.x) min.x = CopyBuffer[i].Pos.x;
				if (CopyBuffer[i].Pos.y < min.y) min.y = CopyBuffer[i].Pos.y;
			}
			for (int i = 0; i < CopyBuffer.size(); i++)
			{
				std::shared_ptr<ScenarioElement> Added = AddElementGUI(CopyBuffer[i].Element,
					ImVec2(CopyBuffer[i].Pos.x - min.x + MousePosInCanvas.x, CopyBuffer[i].Pos.y - min.y + MousePosInCanvas.y),
					CopyBuffer[i].Type, CopyBuffer[i].ElementInStorage);
				if (Added)
				{
					SelectedElems.push_back((int)Elems.size() - 1);
					ToChange[i] = GetNumOfElement(Added);
				}
			}
			for (int k = 0; k < LinksBuffer.size(); k++)
			{
				if (ToChange.find(LinksBuffer[k].Elems[0]) != ToChange.end() && ToChange.find(LinksBuffer[k].Elems[1]) != ToChange.end())
				{
					AddLinkGUI(LinksBuffer[k].Points[0], LinksBuffer[k].Points[1],
						ToChange[LinksBuffer[k].Elems[0]], ToChange[LinksBuffer[k].Elems[1]]);
					SelectedLinks.push_back((int)Links.size() - 1);
				}
			}
			CopyBufferAction("Elem_Group_Add");
		}
	}

	void Delete()
	{
		if (SelectedElems.size() > 0 || SelectedLinks.size() > 0)
		{
			CopyBufferAction("Elem_Group_Delete");
			for (int i = (int)SelectedLinks.size() - 1; i >= 0; i--)
			{
				std::sort(SelectedLinks.begin(), SelectedLinks.end());
				int SelectedLink = SelectedLinks[i];
				DeleteLinkGUI(Links.begin() + SelectedLink);
			}
			for (int i = (int)SelectedElems.size() - 1; i >= 0; i--)
			{
				std::sort(SelectedElems.begin(), SelectedElems.end());
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
			SelectedLinks.clear();
		}
	}

	void DeleteLinks()
	{
		if (SelectedElems.size() > 0)
		{
			CopyBufferAction("Elems_Remove_Links");
			for (int i = (int)SelectedElems.size() - 1; i >= 0; i--)
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
			SelectedLinks.clear();
		}
	}

#pragma endregion Copying, pasting, etc...

#pragma region Scenarios

	// scenarios section
	void DrawScenariosSection(const ImGuiViewport* viewport)
	{
		ImGui::SetNextWindowPos(viewport->WorkPos);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, (viewport->WorkSize.y / 3) - 29));
		ImGui::Begin(u8"Сценарии", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus);
		if (ImGui::Button(u8"Создать сценарий"))
		{
			ScenarioEditorScenarioStorage::CreateNewScenario();
			CheckSave = true;
		}
		ImGui::SameLine();
		std::vector<std::string> Scenarios = ScenarioEditorScenarioStorage::GetScenarioNames();
		if (SelectedElemGUI > 0 && SelectedElemGUI < Scenarios.size())
		{
			if (ImGui::Button(u8"Дублировать выбранный"))
			{
				DoubleSelectedScenario(ScenarioEditorScenarioStorage::GetActualGUID());
				Scenarios = ScenarioEditorScenarioStorage::GetScenarioNames();
				SelectedElemGUI = (int)Scenarios.size() - 1;
				CheckSave = true;
			}
			ImGui::SameLine();
			if (ImGui::Button(u8"Удалить выбранный"))
			{
				RemoveSelectedScenario(ScenarioEditorScenarioStorage::GetActualGUID());
				Scenarios = ScenarioEditorScenarioStorage::GetScenarioNames();
				SelectedElemGUI = 0;
				CheckSave = true;
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
		{
			ImGui::Text((std::string(u8"Выбранный файл: ") + toUtf8(CurrentFile)).c_str());
		}
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
		ScenarioEditorScenarioStorage::SetActualScenario((int)ScenarioEditorScenarioStorage::GetScenarios().size() - 1);
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

#pragma endregion Scenarios window related functions

#pragma region Elems

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
			ImGui::ImageButton("Element", (void*)Tex.Payload, ImVec2((float)Tex.Width, (float)Tex.Height));
			// drag'n'drop source
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Element", &i, sizeof(int), ImGuiCond_Once);
				//ImGui::Text(ElementNames[i]);
				ImGui::ImageButton("Element", (void*)Tex.Payload, ImVec2((float)Tex.Width, (float)Tex.Height));
				ImGui::EndDragDropSource();
			}
			ImGui::PopID();
		}
		ImGui::PopStyleVar();
	}

#pragma endregion Elems window related functions

#pragma region Params

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
				if (ImGui::InputTextMultiline("##Caption", &(Elems[SelectedElems[0]].ElementInStorage)->caption, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * (count + 1))))
					CheckSave = true;
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
						if (ImGui::InputTextMultiline(label.c_str(), &(Attributes[i])->ValueS, ImVec2(-FLT_MIN, ImGui::GetTextLineHeight() * (count + 1))))
						{
							CheckSave = true;
						}
						break;
					case 1:
					{
						int State = (int)(Attributes[i])->ValueF;
						const char* items[] = { "False", "True" };
						if (ImGui::Combo(label.c_str(), &State, items, IM_ARRAYSIZE(items)))
						{
							CheckSave = true;
						}
						(Attributes[i])->ValueF = (float)State;
						break;
					}
					case 2:
						if (ImGui::InputFloat(label.c_str(), &(Attributes[i])->ValueF, 0.01f, 1.0f, "%.2f"))
						{
							CheckSave = true;
						}
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

#pragma endregion Params window related functions

#pragma region Search and map

	void SwitchSearch()
	{
		SearchOpen = !SearchOpen;
	}

	void SwitchMap()
	{
		MapOpen = !MapOpen;
	}

	void SearchWindow()
	{
		if (SearchOpen)
		{
			static const char* LongestTextLabel = u8"Тип поиска";
			float Spacing = ImGui::CalcTextSize(LongestTextLabel).x + ImGui::GetStyle().WindowPadding.x + ImGui::GetStyle().ItemSpacing.x;
			ImGui::Begin(u8"Поиск", &SearchOpen, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);
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
			std::vector<int> SearchResult = DoSearch(CurrentSearchType, CurrentElement, AttributeNames[CurrentElement][CurrentAttribute], SearchField);
			ImGui::Separator();
			if (SearchResult.size() == 0)
				ImGui::Text(u8"Не найдено элементов");
			else
			{
				int Size = (int)SearchResult.size();
				std::string begin, end;
				if (Size % 10 == 1 && Size % 100 != 11)
				{
					begin = u8"Найден ";
					end = u8" элемент";
				}
				else if (Size % 10 > 1 && Size % 10 < 5 && (Size % 100 < 10 || Size % 100 > 20))
				{
					begin = u8"Найдено ";
					end = u8" элемента";
				}
				else
				{
					begin = u8"Найдено ";
					end = u8" элементов";
				}
				ImGui::Text((begin + std::to_string(Size) + end).c_str());
				static int CurrentElem = -1;
				if (ImGui::ArrowButton("##Previous", ImGuiDir_Left))
				{
					if (CurrentElem <= 0) CurrentElem = Size - 1;
					else CurrentElem--;
					float shift_x = ElementsData::GetElementTexture(Elems[SearchResult[CurrentElem]].Element, CanvasZoom).Width / 2.0f;
					float shift_y = ElementsData::GetElementTexture(Elems[SearchResult[CurrentElem]].Element, CanvasZoom).Height / 2.0f;
					CenterView(Elems[SearchResult[CurrentElem]].Pos.x + shift_x, Elems[SearchResult[CurrentElem]].Pos.y + shift_y);
				}
				ImGui::SameLine();
				if (ImGui::ArrowButton("##Next", ImGuiDir_Right))
				{
					if (CurrentElem + 1 > Size - 1) CurrentElem = 0;
					else CurrentElem++;
					float shift_x = ElementsData::GetElementTexture(Elems[SearchResult[CurrentElem]].Element, CanvasZoom).Width / 2.0f;
					float shift_y = ElementsData::GetElementTexture(Elems[SearchResult[CurrentElem]].Element, CanvasZoom).Height / 2.0f;
					CenterView(Elems[SearchResult[CurrentElem]].Pos.x + shift_x, Elems[SearchResult[CurrentElem]].Pos.y + shift_y);
				}
			}
			ImGui::End();
		}
	}

	std::vector<int> DoSearch(int SearchType, int SearchElement, std::string SearchAttribute, std::string SearchField)
	{
		std::vector<int> ret;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		//	Draw list now won't check if elem is out of canvas (called after End())
		if (SearchType == 0)
		{
			if (SearchField.size() == 0) return ret;
			for (int Elem = 0; Elem < Elems.size(); Elem++)
			{
				bool ShouldSelect = false;
				if ((*Elems[Elem].ElementInStorage).caption.find(SearchField) != std::string::npos)
				{
					ShouldSelect = true;
				}
				else
				{
					for (ElementAttribute* Attribute : (*Elems[Elem].ElementInStorage).GetAttributes())
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
					ret.push_back(Elem);
					Texture UsedTexture = ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom);
					float x = origin.x + Elems[Elem].Pos.x * CanvasZoom - 10 > viewport->WorkSize.x / 4 ? origin.x + Elems[Elem].Pos.x * CanvasZoom - 10 : viewport->WorkSize.x / 4;
					if (x < origin.x + Elems[Elem].Pos.x * CanvasZoom + UsedTexture.Width + 10)
						canvas_draw_list->AddRectFilled(
							ImVec2(x, origin.y + Elems[Elem].Pos.y * CanvasZoom - 10),
							ImVec2(origin.x + Elems[Elem].Pos.x * CanvasZoom + UsedTexture.Width + 10, origin.y + Elems[Elem].Pos.y * CanvasZoom + UsedTexture.Height + 10),
							IM_COL32(0, 255, 64, 50));
				}
			}
		}
		else // SearchType == 1
		{
			if (SearchAttribute != u8"<Найти все>" && SearchField.size() == 0) return ret;
			for (int Elem = 0; Elem < Elems.size(); Elem++)
			{
				if (Elems[Elem].Element != SearchElement)
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
					ShouldSelect = (*Elems[Elem].ElementInStorage).caption.find(SearchField) != std::string::npos;
				}
				else
				{
					for (ElementAttribute* Attribute : (*Elems[Elem].ElementInStorage).GetAttributes())
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
					ret.push_back(Elem);
					Texture UsedTexture = ElementsData::GetElementTexture(Elems[Elem].Element, CanvasZoom);
					float x = origin.x + Elems[Elem].Pos.x * CanvasZoom - 10 > viewport->WorkSize.x / 4 ? origin.x + Elems[Elem].Pos.x * CanvasZoom - 10 : viewport->WorkSize.x / 4;
					if (x < origin.x + Elems[Elem].Pos.x * CanvasZoom + UsedTexture.Width + 10)
						canvas_draw_list->AddRectFilled(
							ImVec2(x, origin.y + Elems[Elem].Pos.y * CanvasZoom - 10),
							ImVec2(origin.x + Elems[Elem].Pos.x * CanvasZoom + UsedTexture.Width + 10, origin.y + Elems[Elem].Pos.y * CanvasZoom + UsedTexture.Height + 10),
							IM_COL32(0, 255, 64, 50));
				}
			}
		}
		return ret;
	}

	void MapWindow()
	{
		if (MapOpen)
		{
			std::vector<ImVec2> Corners = GetCorners();
			static const ImVec2 MapMinSize{ 600, 400 };
			static const ImColor RegionColor{ IM_COL32(255,255,255,255) };
			static const ImColor ViewSizeColor{ IM_COL32(150, 150, 150, 100) };
			ImGui::SetNextWindowSizeConstraints(MapMinSize, ImVec2(FLT_MAX, FLT_MAX));   // Aspect ratio
			ImGui::Begin(u8"Карта сценария", &MapOpen, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoScrollbar);
			if (Elems.size() == 0)
			{
				ImGui::End();
				return;
			}
			ImVec2 OldCursorPos = ImGui::GetCursorPos();
			ImDrawList* map_draw_list = ImGui::GetWindowDrawList();
			float MapZoom = min(ImGui::GetWindowWidth() / (Corners[1].x - Corners[0].x + Space * 2), (ImGui::GetWindowHeight() - ImGui::GetFontSize() - ImGui::GetStyle().FramePadding.y * 2) / (Corners[1].y - Corners[0].y + Space * 2));
			ImVec2 map_p0 = {
				ImGui::GetWindowPos().x + (ImGui::GetWindowWidth() - (Corners[1].x - Corners[0].x + Space * 2) * MapZoom) / 2,
				ImGui::GetWindowPos().y + (ImGui::GetWindowHeight() + ImGui::GetFontSize() + ImGui::GetStyle().FramePadding.y * 2 - (Corners[1].y - Corners[0].y + Space * 2) * MapZoom) / 2
			};

			map_draw_list->AddRectFilled(map_p0, { map_p0.x + (Corners[1].x - Corners[0].x + Space * 2) * MapZoom, map_p0.y + (Corners[1].y - Corners[0].y + Space * 2) * MapZoom }, RegionColor);

			ImGui::SetCursorPos({ map_p0.x - ImGui::GetWindowPos().x, map_p0.y - ImGui::GetWindowPos().y });
			ImGui::InvisibleButton("##setviewfield", { (Corners[1].x - Corners[0].x + Space * 2) * MapZoom,(Corners[1].y - Corners[0].y + Space * 2) * MapZoom });
			ImGui::SetCursorPos(OldCursorPos);
			if (ImGui::IsItemActive())
			{
				ImVec2 NewOriginOnMap = ImGui::GetIO().MousePos;
				NewOriginOnMap.x -= (canvas_sz.x / CanvasZoom * MapZoom) / 2;
				NewOriginOnMap.y -= (canvas_sz.y / CanvasZoom * MapZoom) / 2;
				NewOriginOnMap.x = min(NewOriginOnMap.x, (map_p0.x + (Corners[1].x - Corners[0].x + Space * 2) * MapZoom - canvas_sz.x / CanvasZoom * MapZoom));
				NewOriginOnMap.y = min(NewOriginOnMap.y, (map_p0.y + (Corners[1].y - Corners[0].y + Space * 2) * MapZoom - canvas_sz.y / CanvasZoom * MapZoom));
				NewOriginOnMap.x = max(NewOriginOnMap.x, map_p0.x);
				NewOriginOnMap.y = max(NewOriginOnMap.y, map_p0.y);
				scrolling.x = -((NewOriginOnMap.x - map_p0.x) / MapZoom - Space + Corners[0].x + shift.x / CanvasZoom);
				scrolling.y = -((NewOriginOnMap.y - map_p0.y) / MapZoom - Space + Corners[0].y + shift.y / CanvasZoom);
			}

			for (int i = 0; i < Elems.size(); i++)
			{
				Texture UsedTexture = ElementsData::GetElementTexture(Elems[i].Element, MapZoom);
				map_draw_list->AddImage(
					(void*)UsedTexture.Payload, ImVec2(map_p0.x + (Elems[i].Pos.x - Corners[0].x + Space) * MapZoom, map_p0.y + (Elems[i].Pos.y - Corners[0].y + Space) * MapZoom),
					ImVec2(map_p0.x + (Elems[i].Pos.x - Corners[0].x + Space) * MapZoom + UsedTexture.Width, map_p0.y + (Elems[i].Pos.y - Corners[0].y + Space) * MapZoom + UsedTexture.Height));
			}


			for (int j = 0; j < Links.size(); j++)
			{
				map_draw_list->AddLine(
					GetMapLinkingPointLocation(Links[j].Elems[0], Links[j].Points[0], MapZoom, map_p0, -Corners[0].x + Space, -Corners[0].y + Space),
					GetMapLinkingPointLocation(Links[j].Elems[1], Links[j].Points[1], MapZoom, map_p0, -Corners[0].x + Space, -Corners[0].y + Space),
					IM_COL32(0, 0, 0, 255), 0.1f
				);
			}

			ImVec2 ScreenTopLeft = {
				max(-scrolling.x - shift.x / CanvasZoom, Corners[0].x - Space),
				max(-scrolling.y - shift.y / CanvasZoom, Corners[0].y - Space) };
			ImVec2 ScreenBottomRight = {
				min(-scrolling.x - shift.x / CanvasZoom + canvas_sz.x / CanvasZoom, Corners[1].x + Space),
				min(-scrolling.y - shift.y / CanvasZoom + canvas_sz.y / CanvasZoom, Corners[1].y + Space)
			};

			if (ScreenTopLeft.x < ScreenBottomRight.x && ScreenTopLeft.y < ScreenBottomRight.y)
			{
				map_draw_list->AddRectFilled(
					ImVec2(map_p0.x + (ScreenTopLeft.x - Corners[0].x + Space) * MapZoom, map_p0.y + (ScreenTopLeft.y - Corners[0].y + Space) * MapZoom),
					ImVec2(map_p0.x + (ScreenBottomRight.x - Corners[0].x + Space) * MapZoom, map_p0.y + (ScreenBottomRight.y - Corners[0].y + Space) * MapZoom),
					ViewSizeColor);

				map_draw_list->AddRect(
					ImVec2(map_p0.x + (ScreenTopLeft.x - Corners[0].x + Space) * MapZoom, map_p0.y + (ScreenTopLeft.y - Corners[0].y + Space) * MapZoom),
					ImVec2(map_p0.x + (ScreenBottomRight.x - Corners[0].x + Space) * MapZoom, map_p0.y + (ScreenBottomRight.y - Corners[0].y + Space) * MapZoom),
					IM_COL32(0, 0, 0, 255));
			}

			ImGui::End();
		}
	}


#pragma endregion Search and map window related functions

#pragma region Actions buffer

	// Ctrl+Y, Ctrl+Z realization
	bool CopyBufferAction(std::string Operation, float drag_shift_x, float drag_shift_y)
	{
		bool IsFilling = false;
		struct ElementMemory
		{
			int Action;
			ElementOnCanvas Elem;
			int ElementNum;
			ImVec2 OldPos;
		};
		struct LinkMemory
		{
			int Action;
			LinkOnCanvas Link;
			std::shared_ptr<ScenarioElement> ElemA;
			std::shared_ptr<ScenarioElement> ElemB;
		};
		static std::vector<ElementMemory> RememberedElems;
		static std::vector<LinkMemory> RememberedLinks;
		static std::vector<std::string> Operations;
		static int CurrentlyAt = -1;
		if (Operation == "Can_Redo")
		{
			return CurrentlyAt != -1;
		}
		if (Operation == "Can_Undo")
		{
			return Operations.size() > 0 && CurrentlyAt != Operations.size() - 1;
		}
		if (Operation == "Elem_Add")
		{
			RememberedElems.push_back({ -1, Elems[Elems.size() - 1], (int)Elems.size() - 1 });
			IsFilling = true;
		}
		else if (Operation == "Elem_Group_Add")
		{
			for (int i = 0; i < SelectedElems.size(); i++)
			{
				IsFilling = true;
				RememberedElems.push_back({ -1, Elems[SelectedElems[i]], SelectedElems[i] });
			}
			for (int i = 0; i < SelectedLinks.size(); i++)
			{
				IsFilling = true;
				RememberedLinks.push_back({ -1, Links[SelectedLinks[i]],
					Elems[Links[SelectedLinks[i]].Elems[0]].ElementInStorage,
					Elems[Links[SelectedLinks[i]].Elems[1]].ElementInStorage });
			}
		}
		else if (Operation == "Elem_Group_Delete")
		{
			IsFilling = true;
			for (int i = 0; i < SelectedElems.size(); i++)
			{
				RememberedElems.push_back({ -1, Elems[SelectedElems[i]], SelectedElems[i] });
			}
			for (int i = 0; i < SelectedLinks.size(); i++)
			{
				RememberedLinks.push_back({ -1, Links[SelectedLinks[i]],
					Elems[Links[SelectedLinks[i]].Elems[0]].ElementInStorage,
					Elems[Links[SelectedLinks[i]].Elems[1]].ElementInStorage });
			}
			for (int i = 0; i < Links.size(); i++)
			{
				if (
					(
						std::find(SelectedElems.begin(), SelectedElems.end(), Links[i].Elems[0]) != SelectedElems.end() ||
						std::find(SelectedElems.begin(), SelectedElems.end(), Links[i].Elems[1]) != SelectedElems.end()
						) && std::find(SelectedLinks.begin(), SelectedLinks.end(), i) == SelectedLinks.end()
					)
					RememberedLinks.push_back({ -1, Links[i],
					Elems[Links[i].Elems[0]].ElementInStorage,
					Elems[Links[i].Elems[1]].ElementInStorage });
			}
		}
		else if (Operation == "Link_Add")
		{
			RememberedLinks.push_back({ -1, Links[Links.size() - 1],
					Elems[Links[Links.size() - 1].Elems[0]].ElementInStorage,
					Elems[Links[Links.size() - 1].Elems[1]].ElementInStorage });
			IsFilling = true;
		}
		else if (Operation == "Elems_Remove_Links")
		{
			for (int i = 0; i < Links.size(); i++)
			{
				if (
					(
						std::find(SelectedElems.begin(), SelectedElems.end(), Links[i].Elems[0]) != SelectedElems.end() ||
						std::find(SelectedElems.begin(), SelectedElems.end(), Links[i].Elems[1]) != SelectedElems.end()
						)
					)
					RememberedLinks.push_back({ -1, Links[i],
					Elems[Links[i].Elems[0]].ElementInStorage,
					Elems[Links[i].Elems[1]].ElementInStorage });
			}
			IsFilling = true;
		}
		else if (Operation == "Elem_Group_Move")
		{
			IsFilling = true;
			for (int i = 0; i < SelectedElems.size(); i++)
			{
				RememberedElems.push_back({ -1, Elems[SelectedElems[i]], SelectedElems[i], { Elems[SelectedElems[i]].Pos.x - drag_shift_x, Elems[SelectedElems[i]].Pos.y - drag_shift_y} });
			}
		}
		if (IsFilling)
		{
			std::vector<ElementMemory>::iterator k = RememberedElems.begin();
			while (k != RememberedElems.end())
			{
				k->Action++;
				if (k->Action > 9)
				{
					k = RememberedElems.erase(k);
				}
				else
				{
					++k;
				}
			}
			std::vector<LinkMemory>::iterator j = RememberedLinks.begin();
			while (j != RememberedLinks.end())
			{
				j->Action++;
				if (j->Action > 9)
				{
					j = RememberedLinks.erase(j);
				}
				else
				{
					++j;
				}
			}
			Operations.insert(Operations.begin(), Operation);
			if (Operations.size() > 10)
			{
				Operations.erase(Operations.end() - 1);
			}
			for (int i = 0; i <= CurrentlyAt; i++)
			{
				for (int j = 0; j < RememberedLinks.size(); j++)
				{
					if (RememberedLinks[j].Action == 0) continue;
					if (RememberedLinks[j].Action - 1 == 0)
					{
						RememberedLinks.erase(RememberedLinks.begin() + j--);
					}
					else RememberedLinks[j].Action--;
				}
				for (int j = 0; j < RememberedElems.size(); j++)
				{
					if (RememberedElems[j].Action == 0) continue;
					if (RememberedElems[j].Action - 1 == 0)
					{
						RememberedElems.erase(RememberedElems.begin() + j--);
					}
					else
						RememberedElems[j].Action--;
				}
				if (Operations.size() > 1)
					Operations.erase(Operations.begin() + 1);

			}
			CurrentlyAt = -1;
		}
		if (Operation == "Clear")
		{
			RememberedElems.clear();
			RememberedLinks.clear();
			Operations.clear();
			CurrentlyAt = -1;
		}
		else if (Operation == "Undo")
		{
			SelectedElems.clear();
			SelectedLinks.clear();
			if (CurrentlyAt == 9 || CurrentlyAt + 1 == (int)Operations.size()) return 0;
			CurrentlyAt++;
			if (Operations[CurrentlyAt] == "Elem_Add")
			{
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						int NumOfElem = GetNumOfElement(RememberedElems[i].Elem.ElementInStorage);
						for (int j = 0; j < Links.size(); j++)
						{
							if (Links[j].Elems[0] > NumOfElem)
							{
								Links[j].Elems[0]--;
							}
							if (Links[j].Elems[1] > NumOfElem)
							{
								Links[j].Elems[1]--;
							}
						}
						DeleteElementGUI(Elems.begin() + GetNumOfElement(RememberedElems[i].Elem.ElementInStorage));
						break;
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elem_Group_Add")
			{
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						for (int j = 0; j < Links.size(); j++)
						{
							if ((Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemB)
								&& (Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemB)
								)
							{
								DeleteLinkGUI(Links.begin() + j);
							}
						}
					}
				}
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						int NumOfElem = GetNumOfElement(RememberedElems[i].Elem.ElementInStorage);
						for (int j = 0; j < Links.size(); j++)
						{
							if (Links[j].Elems[0] > NumOfElem)
							{
								Links[j].Elems[0]--;
							}
							if (Links[j].Elems[1] > NumOfElem)
							{
								Links[j].Elems[1]--;
							}
						}
						DeleteElementGUI(Elems.begin() + NumOfElem);
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elem_Group_Delete")
			{
				std::vector<std::vector<std::shared_ptr<ScenarioElement>>> ToChange;
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						AddElementGUI(RememberedElems[i].Elem.Element, RememberedElems[i].Elem.Pos, RememberedElems[i].Elem.Type, RememberedElems[i].Elem.ElementInStorage);
						std::vector<std::shared_ptr<ScenarioElement>> OldToNew{ RememberedElems[i].Elem.ElementInStorage, Elems[Elems.size() - 1].ElementInStorage };
						ToChange.push_back(OldToNew);
					}
				}
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					for (int j = 0; j < ToChange.size(); j++)
					{
						if (RememberedElems[i].Elem.ElementInStorage == ToChange[j][0])
							RememberedElems[i].Elem.ElementInStorage = ToChange[j][1];
					}
				}
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					for (int j = 0; j < ToChange.size(); j++)
					{
						if (RememberedLinks[i].ElemA == ToChange[j][0])
							RememberedLinks[i].ElemA = ToChange[j][1];
						if (RememberedLinks[i].ElemB == ToChange[j][0])
							RememberedLinks[i].ElemB = ToChange[j][1];
					}
				}
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						int First = GetNumOfElement(RememberedLinks[i].ElemA);
						int Second = GetNumOfElement(RememberedLinks[i].ElemB);
						AddLinkGUI(RememberedLinks[i].Link.Points[0], RememberedLinks[i].Link.Points[1], First, Second);
					}
				}
			}
			if (Operations[CurrentlyAt] == "Link_Add")
			{
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						for (int j = 0; j < Links.size(); j++)
						{
							if ((Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemB)
								&& (Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemB)
								)
								DeleteLinkGUI(Links.begin() + j);
						}
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elems_Remove_Links")
			{
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						int First = GetNumOfElement(RememberedLinks[i].ElemA);
						int Second = GetNumOfElement(RememberedLinks[i].ElemB);
						AddLinkGUI(RememberedLinks[i].Link.Points[0], RememberedLinks[i].Link.Points[1], First, Second);
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elem_Group_Move")
			{
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						Elems[GetNumOfElement(RememberedElems[i].Elem.ElementInStorage)].Pos = RememberedElems[i].OldPos;
						ScenariosEditorScenarioElement::UpdateCoordinates(
							RememberedElems[i].Elem.ElementInStorage,
							RememberedElems[i].OldPos.x,
							RememberedElems[i].OldPos.y
						);
					}
				}
			}
			CheckSave = true;
			return false;
		}
		else if (Operation == "Redo")
		{
			SelectedElems.clear();
			SelectedLinks.clear();
			if (CurrentlyAt < 0) return 0;
			if (Operations[CurrentlyAt] == "Elem_Add")
			{
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						AddElementGUI(RememberedElems[i].Elem.Element, RememberedElems[i].Elem.Pos, RememberedElems[i].Elem.Type, RememberedElems[i].Elem.ElementInStorage);
						std::vector<std::shared_ptr<ScenarioElement>> ToChange = { RememberedElems[i].Elem.ElementInStorage, Elems[Elems.size() - 1].ElementInStorage };
						RememberedElems[i].Elem.ElementInStorage = Elems[Elems.size() - 1].ElementInStorage;
						for (int i = 0; i < RememberedElems.size(); i++)
						{
							if (RememberedElems[i].Elem.ElementInStorage == ToChange[0])
								RememberedElems[i].Elem.ElementInStorage = ToChange[1];
						}
						for (int i = 0; i < RememberedLinks.size(); i++)
						{
							for (int j = 0; j < ToChange.size(); j++)
							{
								if (RememberedLinks[i].ElemA == ToChange[0])
									RememberedLinks[i].ElemA = ToChange[1];
								if (RememberedLinks[i].ElemB == ToChange[0])
									RememberedLinks[i].ElemB = ToChange[1];
							}
						}
						break;
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elem_Group_Add")
			{
				std::vector<std::vector<std::shared_ptr<ScenarioElement>>> ToChange;
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						AddElementGUI(RememberedElems[i].Elem.Element, RememberedElems[i].Elem.Pos, RememberedElems[i].Elem.Type, RememberedElems[i].Elem.ElementInStorage);
						std::vector<std::shared_ptr<ScenarioElement>> OldToNew{ RememberedElems[i].Elem.ElementInStorage, Elems[Elems.size() - 1].ElementInStorage };
						ToChange.push_back(OldToNew);
					}
				}
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					for (int j = 0; j < ToChange.size(); j++)
					{
						if (RememberedElems[i].Elem.ElementInStorage == ToChange[j][0])
							RememberedElems[i].Elem.ElementInStorage = ToChange[j][1];
					}
				}
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					for (int j = 0; j < ToChange.size(); j++)
					{
						if (RememberedLinks[i].ElemA == ToChange[j][0])
							RememberedLinks[i].ElemA = ToChange[j][1];
						if (RememberedLinks[i].ElemB == ToChange[j][0])
							RememberedLinks[i].ElemB = ToChange[j][1];
					}
				}
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						int First = GetNumOfElement(RememberedLinks[i].ElemA);
						int Second = GetNumOfElement(RememberedLinks[i].ElemB);
						AddLinkGUI(RememberedLinks[i].Link.Points[0], RememberedLinks[i].Link.Points[1], First, Second);
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elem_Group_Delete")
			{
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						for (int j = 0; j < Links.size(); j++)
						{
							if ((Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemB)
								&& (Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemB)
								)
								DeleteLinkGUI(Links.begin() + j);
						}
					}
				}
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						int NumOfElem = GetNumOfElement(RememberedElems[i].Elem.ElementInStorage);
						for (int j = 0; j < Links.size(); j++)
						{
							if (Links[j].Elems[0] > NumOfElem)
							{
								Links[j].Elems[0]--;
							}
							if (Links[j].Elems[1] > NumOfElem)
							{
								Links[j].Elems[1]--;
							}
						}
						DeleteElementGUI(Elems.begin() + NumOfElem);
					}
				}

			}
			if (Operations[CurrentlyAt] == "Link_Add")
			{
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						int First = GetNumOfElement(RememberedLinks[i].ElemA);
						int Second = GetNumOfElement(RememberedLinks[i].ElemB);
						AddLinkGUI(RememberedLinks[i].Link.Points[0], RememberedLinks[i].Link.Points[1], First, Second);
						break;
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elems_Remove_Links")
			{
				for (int i = 0; i < RememberedLinks.size(); i++)
				{
					if (RememberedLinks[i].Action == CurrentlyAt)
					{
						for (int j = 0; j < Links.size(); j++)
						{
							if ((Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[0]].ElementInStorage == RememberedLinks[i].ElemB)
								&& (Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemA || Elems[Links[j].Elems[1]].ElementInStorage == RememberedLinks[i].ElemB)
								)
								DeleteLinkGUI(Links.begin() + j);
						}
					}
				}
			}
			if (Operations[CurrentlyAt] == "Elem_Group_Move")
			{
				for (int i = 0; i < RememberedElems.size(); i++)
				{
					if (RememberedElems[i].Action == CurrentlyAt)
					{
						Elems[GetNumOfElement(RememberedElems[i].Elem.ElementInStorage)].Pos = RememberedElems[i].Elem.Pos;
						ScenariosEditorScenarioElement::UpdateCoordinates(
							RememberedElems[i].Elem.ElementInStorage,
							RememberedElems[i].Elem.Pos.x,
							RememberedElems[i].Elem.Pos.y
						);
					}
				}
			}
			CurrentlyAt--;
			CheckSave = true;
		}
		return false;
	}

#pragma endregion Function, which implements Ctrl+Z, Ctrl+Y functionality
	
#pragma region Header functions

	// Hooked from main, starts scenarios editor window
	void ShowDemoScenarioGUI()
	{
		ScenariosEditorRender::InitializeWindowAndImGUIContext();
		PreLoopSetup();
		MainLoop();
		ScenariosEditorRender::EraseWindowAndImGuiContext();
	}

	// Adds element (e.g. from storage), this one doesn't create new object in elements storage
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

	// Adds link (e.g. from sstorage), this one doesn't create new object in links storage
	void AddLink(int element_a_index, int element_b_index, int element_a_point, int element_b_point)
	{
		Links.push_back({ {element_a_point, element_b_point} , {element_a_index, element_b_index } });
	}

	// Returns name of element
	const char* GetNameOfElementOnCanvas(int index)
	{
		return ElementsData::GetElementName(Elems[index].Element);
	}

#pragma endregion Functions that are used only from other files

}