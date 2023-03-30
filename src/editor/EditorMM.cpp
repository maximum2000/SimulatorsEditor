// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_internal.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#include <list>
#include <vector>
#include "TextureLoader.h"
#include "Render.h"
#include "CanvasElements.h"
#include "CanvasLinks.h"
#include <ctime>
#include "ColorData.h"
#include <math.h>
#include <string>

namespace EditorMathModel
{
	enum ProgrammState
	{
		/* 0*/Rest,
		/* 1*/CanvasSelection,
		/* 2*/ElementSelection,
		/* 3*/ElementWindowSelection,
		/* 4*/CanvasMapWindowSelection,
		/* 5*/TopBarSelection,
		/* 6*/BottomBarSelection,
		/* 7*/ElementHover,
		/* 8*/RectangleSelection,
		/* 9*/RectangleSelectionPlus,
		/*10*/ElementDrag,
		/*11*/CanvasDrag,
		/*12*/LinkDotHover,
		/*13*/LinkCreate,
		/*14*/LinkHover,
		/*15*/LinkSelected
	};

	// Forward declarations of helper functions
	void CreateDemoScenarioGUI();

#pragma region State Machine (declaration)
	void StateMachineSetState(ProgrammState newState);
	void StateMachineLogic(ImGuiIO& io);
#pragma endregion
#pragma region Draw Blocks of interface (declaration)
	void DrawTopBar();
	void DrawElementsWindow();
	void DrawCanvasMapWindow();
	void DrawCanvas();
	void DrawBottomBar();
#pragma endregion
#pragma region Additional draws (declaration)
	void CanvasDrawElements(ImGuiIO& io);
	void CanvasDrawLinkingPoints();
	void CanvasDrawLinks();
	void CanvasElementRenderRect(ImVec2 startPosition, ImVec2 endPosition, ImU32 colorBorder, ImU32 colorFill);
	void DrawDragNDropWindow();
#pragma endregion
#pragma region Logic functions (declaration)
	void SearchLogic(char data[]);
#pragma endregion
#pragma region Helper functions for canvas (declaration)
	void CanvasDragging(ImVec2 moveDelta);
	void CanvasScaling(float deltaValue, ImGuiIO& io);
#pragma endregion
#pragma region Helper function for Rectangle Selection (declaration)
	void CanvasRectangleSelection(ImGuiIO& io, ImVec2 SelectionStartPosition);
	void SelectElementsInsideRectangle(ImGuiIO& io, ImVec2 start);
	void UnselectElementsInsideRectangle(ImGuiIO& io, ImVec2 start);
#pragma endregion
#pragma region Helper functions for Canvas Elements list (declaration)
	void CanvasElementsSelectAll();
	void ClearCanvasSelectedElementsAll();
	void ResetCanvasSelectedElementsAll();
	void SetCanvasSelectedElementsBlockStatus(bool newValue);
	void CalculateSelectedCanvasElements();
	void CanvasElementDelete(int countOfDeleteOperation);
	void CanvasElementsDragByValue(float xDelta, float yDelta);
	void CanvasElementsSearchClear();
#pragma endregion
#pragma region Helper function for linking point location (definition)
	ImVec2 GetLinkingPointLocation(int Elem, int Point);
#pragma endregion
#pragma region Helper functions for links (declaration)
	void CanvasAddLinkingPointsButtons();
	void CanvasAddLinkingPointsButton(int elem, int pin, const char* label, ImVec2 position);
	void CanvasCreateLink(int first_element, int first_pin, int second_element, int second_pin);
	void CanvasDrawLineFromPointToMouse(ImVec2 point);
#pragma endregion
	float ReverseScale(float value);
#pragma region Debug (declaration)
	std::string DEBUGstate(int index);
	void CanvasCenterRectangle();
#pragma endregion
	void LinkRecalculate(CanvasElement* element, ImVec2 delta);

	// Forward declarations of variables
	bool show_elements_window = false;
	bool show_canvas_map_window = false;
	static ImVec2 mousePosition, origin;
	int selectedElementsCount = 0;
	static int CurrentHoveredElementIndex = -1;
	static int CurrentHoveredLinkingPointElementIndex = -1;
	static int CurrentHoveredLinkingPointPinIndex = -1;
	static ImVec2 mousePositionOnClick;
	static float canvasScaleFactor = 1.0f;
	static float topBarHeight;

	// ImGui data
	const ImGuiViewport* viewport;
	ImDrawList* draw_list;

	// Forward declarations of classes and structures
	EditorMMTextureLoader::TextureLoader TextureLoader;
	EditorMMColorData::ColorData ColorData;
	static std::vector<CanvasElement> CanvasElements;
	static std::vector<CanvasLink> CanvasLinks;
	ProgrammState currentState = Rest;

	void CreateDemoScenarioGUI()
	{

		EditorMMRender::Prepare();

		// Our state
		bool show_demo_window = true;
		bool show_another_window = false;

		TextureLoader.LoadToList();

		viewport = ImGui::GetMainViewport();
		ImGuiIO& io = ImGui::GetIO();
		// Main loop
		bool done = false;
		while (!done)
		{
			// Poll and handle messages (inputs, window resize, etc.)
			// See the WndProc() function below for our to dispatch events to the Win32 backend.
			MSG msg;
			while (::PeekMessage(&msg, NULL, 0U, 0U, PM_REMOVE))
			{
				::TranslateMessage(&msg);
				::DispatchMessage(&msg);
				if (msg.message == WM_QUIT)
					done = true;
			}
			if (done)
				break;
			// Start the Dear ImGui frame
			ImGui_ImplDX11_NewFrame();
			ImGui_ImplWin32_NewFrame();
			ImGui::NewFrame();
			// 1. Show the big demo window (Most of the sample code is in ImGui::ShowDemoWindow()! You can browse its code to learn more about Dear ImGui!).
			if (show_demo_window) ImGui::ShowDemoWindow(&show_demo_window);
			//ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
			ImVec4 clear_color = ImVec4(0.01f, 0.01f, 0.01f, 1.00f);
			// 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
			/* {
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!", &show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
				ImGui::Checkbox("Demo Window", &show_demo_window);      // Edit bools storing our window open/close state
				ImGui::Checkbox("Another Window", &show_another_window);

				ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
				ImGui::ColorEdit3("clear color", (float*)&clear_color); // Edit 3 floats representing a color

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}*/
			if (show_elements_window)
			{
				DrawElementsWindow();
			}
			if (show_canvas_map_window)
			{
				DrawCanvasMapWindow();
			}
			DrawTopBar();
			DrawCanvas();
			DrawBottomBar();
			/*if (isDragElementFromWindow && dragElementTextureIndex != -1)
			{
				RenderDragNDropElementWhileDragging();
			}*/
			CalculateSelectedCanvasElements();
			StateMachineLogic(io);
			EditorMMRender::Render();
		}
		EditorMMRender::Cleanup();
	}

#pragma region State Machine (definition)
	void StateMachineSetState(ProgrammState newState)
	{
		if (currentState < 8 || currentState == 12)
		{
			currentState = newState;
		}
		else if (newState == Rest)
		{
			currentState = newState;
		}
	}
	void StateMachineLogic(ImGuiIO& io)
	{
		//static clock_t startTimerLeftMouseButton;
		//static double leftMouseButtonTimeHold = 100;
		//startTimerLeftMouseButton = clock();
		//leftMouseButtonTimeHold = (((double)clock() - startTimerLeftMouseButton) / CLOCKS_PER_SEC);
		//leftMouseButtonTimeHold = 100;
		if (currentState == ElementHover)
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				mousePositionOnClick = io.MousePos;
				if (io.KeyShift)
				{
					CanvasElements[CurrentHoveredElementIndex].isSelected = !CanvasElements[CurrentHoveredElementIndex].isSelected;
				}
				else
				{
					if (CanvasElements[CurrentHoveredElementIndex].isSelected == false)
					{
						ClearCanvasSelectedElementsAll();
					}
					CanvasElements[CurrentHoveredElementIndex].isSelected = true;
				}
				StateMachineSetState(ElementDrag);
			}
		}
		if (currentState == ElementDrag)
		{
			CanvasElementsDragByValue(io.MouseDelta.x, io.MouseDelta.y);
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				if (mousePositionOnClick.x == io.MousePos.x && !io.KeyShift)
				{
					ClearCanvasSelectedElementsAll();
					CanvasElements[CurrentHoveredElementIndex].isSelected = true;
				}
				StateMachineSetState(Rest);
			}
		}
		if (currentState == CanvasSelection)
		{
			CanvasScaling(io.MouseWheel, io);
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				mousePositionOnClick = io.MousePos;
				if (io.KeyShift)
				{
					SetCanvasSelectedElementsBlockStatus(true);
				}
				StateMachineSetState(RectangleSelection);
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				StateMachineSetState(CanvasDrag);
			}
		}
		if (currentState == RectangleSelection)
		{
			SelectElementsInsideRectangle(io, mousePositionOnClick);
			UnselectElementsInsideRectangle(io, mousePositionOnClick);
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				SetCanvasSelectedElementsBlockStatus(false);
				StateMachineSetState(Rest);
			}
		}
		if (currentState == CanvasDrag)
		{
			CanvasDragging(io.MouseDelta);
			/*ImVec2 newOrigin = ImVec2(origin.x + io.MouseDelta.x, origin.y + io.MouseDelta.y);
			if (newOrigin.x > 0)
			{
				newOrigin.x = 0;
			}
			if (newOrigin.y > 0)
			{
				newOrigin.y = 0;
			}
			origin = newOrigin;*/
			if (ImGui::IsMouseReleased(ImGuiMouseButton_Right))
			{
				StateMachineSetState(Rest);
			}
		}
		if (currentState == LinkCreate)
		{
			static int BeginElemIndex = -1;
			static int BeginElemPin = -1;
			if (BeginElemIndex == -1)
			{
				BeginElemIndex = CurrentHoveredLinkingPointElementIndex;
				BeginElemPin = CurrentHoveredLinkingPointPinIndex;
			}
			CanvasDrawLineFromPointToMouse(GetLinkingPointLocation(BeginElemIndex, BeginElemPin));
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && CurrentHoveredLinkingPointElementIndex != -1)
			{
				CanvasCreateLink(BeginElemIndex, BeginElemPin, CurrentHoveredLinkingPointElementIndex, CurrentHoveredLinkingPointPinIndex);
				BeginElemIndex = -1;
				BeginElemPin = -1;
				StateMachineSetState(Rest);
			}
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			{
				BeginElemIndex = -1;
				BeginElemPin = -1;
				StateMachineSetState(Rest);
			}
		}
		if (currentState == LinkDotHover)
		{
			if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
			{
				StateMachineSetState(LinkCreate);
			}
		}
		if (ImGui::IsKeyPressed(ImGuiKey_Delete))
		{
			if (currentState < 8)
			{
				CanvasElementDelete(selectedElementsCount);
			}
		}
		if (ImGui::IsKeyPressed(ImGuiKey_A))
		{
			if (io.KeyCtrl)
			{
				CanvasElementsSelectAll();
			}
		}
	}
#pragma endregion
#pragma region Draw Blocks of interface (definition)
	void DrawElementsWindow()
	{
		ImGui::SetNextWindowSize(ImVec2(400, 400), 0);
		ImGui::Begin("Elements", 0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
		for (int i = 0; i < TextureLoader.GetTextureCount(); i++)
		{
			EditorMMTextureLoader::LoadedTexture Temp = TextureLoader.GetTextureByIndex(i);
			ImGui::SameLine();
			if (ImGui::GetStyle().ItemSpacing.x + Temp.imageWidth > ImGui::GetContentRegionAvail().x)
			{
				ImGui::NewLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ImGui::GetStyle().ItemSpacing.x, ImGui::GetCursorPos().y));
			}
			ImGui::PushID(i);
			if (ImGui::ImageButton("Element", (void*)Temp.myTexture, ImVec2(Temp.imageWidth, Temp.imageHeight)));
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Element", &i, sizeof(int), ImGuiCond_Once);
				ImGui::Text("Payload data is: %d", *(int*)ImGui::GetDragDropPayload()->Data);
				ImGui::EndDragDropSource();
			}
			ImGui::PopID();
		}
		ImGuiIO& io = ImGui::GetIO();
		//ImGui::Text("hovered = %d", CanvasElementsHovered.size());
		ImGui::Text("State: %s", DEBUGstate(currentState));
		ImGui::Text("State: %d", currentState);
		ImGui::Text("Hovered index: %d", CurrentHoveredElementIndex);
		ImGui::Separator();
		ImGui::Text("Origin X: %f", origin.x);
		ImGui::Text("Origin Y: %f", origin.y);
		ImGui::Separator();
		//ImGui::Text("Mouse start X: %f", mousePositionOnClick.x);
		//ImGui::Text("Mouse start Y: %f", mousePositionOnClick.y);
		//ImGui::Text("Mouse current X: %f", io.MousePos.x);
		//ImGui::Text("Mouse current Y: %f", io.MousePos.y);
		//ImGui::Separator();
		ImGui::Text("Scale factor: %f", canvasScaleFactor);
		ImGui::Text("Scale mouse wheel: %f", ImGui::GetIO().MouseWheel);
		ImGui::Separator();
		//ImGui::Text("Selected: |%s|", searchInput);
		if (CanvasElements.size() > 0)
		{
			ImGui::Text("Element X: %f", (CanvasElements[0].centerPosition.x + origin.x) * canvasScaleFactor);
			ImGui::Text("Element Y: %f", (CanvasElements[0].centerPosition.y + origin.y) * canvasScaleFactor);
		}
		if (ImGui::IsWindowHovered())
		{
			//currentState = ElementWindowSelection;
			StateMachineSetState(ElementWindowSelection);
		}
		ImGui::End();
	}
	void DrawCanvasMapWindow()
	{

	}
	void DrawTopBar()
	{
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y;
		ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, footer_height_to_reserve));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowMinSize, ImVec2(0.f, 0.f));
		ImGui::Begin("Main Window", 0, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
		ImGui::PopStyleVar();
		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("New")) {}
				if (ImGui::MenuItem("Open", "Ctrl+O")) {}
				//if (ImGui::BeginMenu("Open Recent")){}
				if (ImGui::MenuItem("Save", "Ctrl+S")) {}
				if (ImGui::MenuItem("Save As..")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Import")) {}
				ImGui::Separator();
				if (ImGui::MenuItem("Exit")) {}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Edit"))
			{
				if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
				if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
				ImGui::Separator();
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Window"))
			{
				if (ImGui::MenuItem("Elements window"))
				{
					show_elements_window = !show_elements_window;
				}
				if (ImGui::MenuItem("Canvas map window"))
				{
					show_canvas_map_window = !show_canvas_map_window;
				}
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Options"))
			{
				ImGui::EndMenu();
			}
			ImGui::EndMenuBar();
		}
		if (ImGui::IsWindowHovered())
		{
			StateMachineSetState(TopBarSelection);
		}
		ImGui::End();
	}
	void DrawCanvas()
	{
		ImGuiIO& io = ImGui::GetIO();
		const float footer_height_to_reserve = ImGui::GetFrameHeightWithSpacing();
		topBarHeight = footer_height_to_reserve - ImGui::GetStyle().ItemSpacing.y;
		// Canvas size and style
		ImGui::SetNextWindowPos(ImVec2(0, footer_height_to_reserve - ImGui::GetStyle().ItemSpacing.y));
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y - footer_height_to_reserve * 2));
		ImGui::Begin("MainWorkspace", NULL,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoScrollWithMouse);
		// Canvas positioning
		static ImVec2 scrolling(0.0f, 0.0f); // current scrolling
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		draw_list = ImGui::GetWindowDrawList();
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(0, 0, 0, 0));
		ImGui::InvisibleButton("canvas", canvas_sz,
			ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight); // Window itself doesn't trigger io mouse actions, invisible button does
		ImGui::SetItemAllowOverlap();
		if (ImGui::IsItemHovered())
		{
			StateMachineSetState(CanvasSelection);
		}
		if (currentState == RectangleSelection)
		{
			CanvasRectangleSelection(io, mousePositionOnClick);
		}
		// Draw grid
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		{
			const float GRID_STEP = 64.0f * canvasScaleFactor;
			for (float x = fmodf(origin.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
			{
				draw_list->AddLine(
					ImVec2(canvas_p0.x + x, canvas_p0.y),
					ImVec2(canvas_p0.x + x, canvas_p1.y),
					IM_COL32(200, 200, 200, 119));
			}
			for (float y = fmodf(origin.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
			{
				draw_list->AddLine(
					ImVec2(canvas_p0.x, canvas_p0.y + y),
					ImVec2(canvas_p1.x, canvas_p0.y + y),
					IM_COL32(200, 200, 200, 119));
			}
		}
		draw_list->PopClipRect();
		// canvas is drag'n'drop reciever
		if (ImGui::BeginDragDropTarget())
		{

			auto payload = ImGui::AcceptDragDropPayload("Element");
			if (payload != NULL)
			{
				int ElementNum = *(int*)payload->Data;
				ImVec2 mousePosition = io.MousePos;
				int x = mousePosition.x - origin.x - TextureLoader.GetTextureByIndex(ElementNum).imageWidth / 2;
				//int x = io.MousePos.x - origin.x - TextureLoader.GetTextureByIndex(ElementNum).imageWidth / 2;
				//int y = io.MousePos.y - origin.y - TextureLoader.GetTextureByIndex(ElementNum).imageHeight / 2;
				int y = mousePosition.y - origin.y - TextureLoader.GetTextureByIndex(ElementNum).imageHeight / 2;
				if (x < 0)
				{
					x = 0;
					mousePosition.x = origin.x + TextureLoader.GetTextureByIndex(ElementNum).imageWidth / 2;
				}
				if (y < 0)
				{
					y = 0;
					mousePosition.y = origin.y + TextureLoader.GetTextureByIndex(ElementNum).imageHeight / 2;
				}
				else
				{
					y -= topBarHeight;
				}
				mousePosition.x -= origin.x;
				mousePosition.y -= origin.y;
				mousePosition.y -= topBarHeight;
				//std::string elemName = TextureLoader.GetTextureNameByIndex(ElementNum);
				//CanvasElements.push_back({ elemName, ElementNum, ImVec2(x, y), mousePosition });
				CanvasElements.push_back({ ElementNum, (short)(15), ImVec2(x, y), mousePosition });
			}
			ImGui::EndDragDropTarget();
		}
		CanvasDrawLinkingPoints();
		CurrentHoveredLinkingPointElementIndex = -1;
		CurrentHoveredLinkingPointPinIndex = -1;
		CanvasAddLinkingPointsButtons();
		CanvasDrawLinks();
		CanvasDrawElements(io);
		CanvasCenterRectangle();
		ImGui::End();
	}
	void DrawBottomBar()
	{
		const float footer_height_to_reserve = ImGui::GetStyle().ItemSpacing.y + ImGui::GetFrameHeightWithSpacing();
		ImGui::SetNextWindowPos(ImVec2(0, ImGui::GetIO().DisplaySize.y - footer_height_to_reserve)); // CHANGE 20 TO DYMANIC
		ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, footer_height_to_reserve));
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("BottomMenuBar", NULL,
			ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus
			| ImGuiWindowFlags_NoScrollWithMouse);
		ImGui::PopStyleVar();
		ImGui::Separator();
		ImGui::Text(" Selected elements: %d |", selectedElementsCount);
		ImGui::SameLine();
		static char searchInput[64] = "";
		static float searchInputWidth = viewport->WorkSize.x / 10;
		float searchItemPosition = searchInputWidth + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SameLine(ImGui::GetWindowWidth() - searchItemPosition);
		ImGui::PushItemWidth(searchInputWidth);
		//ImGui::InputText("", searchInput, 64);
		ImGui::InputText("1", searchInput, 64);
		searchItemPosition += ImGui::CalcTextSize("Search:").x + ImGui::GetStyle().ItemSpacing.x;
		ImGui::SameLine(ImGui::GetWindowWidth() - searchItemPosition);
		ImGui::Text("Search:");
		if (ImGui::IsWindowHovered())
		{
			StateMachineSetState(BottomBarSelection);
		}
		ImGui::End();
		SearchLogic(searchInput);
	}
#pragma endregion
#pragma region Additional draws (definition)
	void CanvasDrawElements(ImGuiIO& io)
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			EditorMMTextureLoader::LoadedTexture UsedTexture = TextureLoader.GetTextureByIndex(CanvasElements[i].elementDataNumber);
			draw_list->AddImage((void*)UsedTexture.myTexture,
				ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor,
					(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight),
				ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor + UsedTexture.imageWidth * canvasScaleFactor,
					(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight + UsedTexture.imageHeight * canvasScaleFactor));
			ImGui::SetCursorScreenPos(ImVec2(
				(origin.x + CanvasElements[i].position.x) * canvasScaleFactor,
				(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight));
			ImGui::InvisibleButton("canvas123",
				ImVec2(UsedTexture.imageWidth * canvasScaleFactor, UsedTexture.imageHeight * canvasScaleFactor),
				ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			ImGui::SetItemAllowOverlap();
			if ((CurrentHoveredElementIndex == -1 || CurrentHoveredElementIndex == i) && currentState < 8)
			{
				if (ImGui::IsItemHovered())
				{
					CurrentHoveredElementIndex = i;
					StateMachineSetState(ElementHover);
					CanvasElementRenderRect(
						ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor,
							(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight),
						ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor + UsedTexture.imageWidth * canvasScaleFactor,
							(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight + UsedTexture.imageHeight * canvasScaleFactor),
						ColorData.CanvasElementHoverColorRect, ColorData.CanvasElementHoverColorRectFill);
					ImGui::SetTooltip("Center X position: %f", CanvasElements[i].centerPosition.x);
				}
				else
				{
					CurrentHoveredElementIndex = -1;
				}
			}
			if (CanvasElements[i].isSearched && !CanvasElements[i].isSelected && CurrentHoveredElementIndex != i)
			{
				CanvasElementRenderRect(
					ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor,
						(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight),
					ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor + UsedTexture.imageWidth * canvasScaleFactor,
						(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight + UsedTexture.imageHeight * canvasScaleFactor),
					ColorData.CanvasElementSearchColorRect, ColorData.CanvasElementSearchColorRectFill);
			}
			if (CanvasElements[i].isSelected)
			{
				CanvasElementRenderRect(
					ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor,
						(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight),
					ImVec2((origin.x + CanvasElements[i].position.x) * canvasScaleFactor + UsedTexture.imageWidth * canvasScaleFactor,
						(origin.y + CanvasElements[i].position.y) * canvasScaleFactor + topBarHeight + UsedTexture.imageHeight * canvasScaleFactor),
					ColorData.CanvasElementSelectColorRect, ColorData.CanvasElementSelectColorRectFill);
			}
		}
	}
	void CanvasElementRenderRect(ImVec2 startPosition, ImVec2 endPosition, ImU32 colorBorder, ImU32 colorFill)
	{
		draw_list->AddRect(
			ImVec2(startPosition.x, startPosition.y),
			ImVec2(endPosition.x, endPosition.y),
			colorBorder);
		draw_list->AddRectFilled(
			ImVec2(startPosition.x, startPosition.y),
			ImVec2(endPosition.x, endPosition.y),
			colorFill);
	}
	void CanvasDrawLinkingPoints()
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (CanvasElements[i].elementDataType & 1)
			{
				draw_list->AddCircleFilled(GetLinkingPointLocation(i, 0), 5.0f, IM_COL32(255, 255, 255, 255));
			}
			if (CanvasElements[i].elementDataType & 2)
			{
				draw_list->AddCircleFilled(GetLinkingPointLocation(i, 1), 5.0f, IM_COL32(255, 255, 255, 255));
			}
			if (CanvasElements[i].elementDataType & 4)
			{
				draw_list->AddCircleFilled(GetLinkingPointLocation(i, 2), 5.0f, IM_COL32(255, 255, 255, 255));
			}
			if (CanvasElements[i].elementDataType & 8)
			{
				draw_list->AddCircleFilled(GetLinkingPointLocation(i, 3), 5.0f, IM_COL32(255, 255, 255, 255));
			}
		}
	}
	void CanvasDrawLinks()
	{
		for (int i = 0; i < CanvasLinks.size(); i++)
		{
			for (int j = 1; j < CanvasLinks[i].linkDots.size(); j++)
			{
				ImVec2 FirstPoint { CanvasLinks[i].linkDots[j - 1].x + origin.x, CanvasLinks[i].linkDots[j - 1].y + origin.y };
				ImVec2 SecondPoint { CanvasLinks[i].linkDots[j].x + origin.x, CanvasLinks[i].linkDots[j].y + origin.y };
				draw_list->AddLine(FirstPoint, SecondPoint, IM_COL32(255, 255, 255, 255));
			}
		}
	}
	void DrawDragNDropWindow()
	{

	}
#pragma endregion
#pragma region Logic functions (definition)
	void SearchLogic(char data[])
	{
		std::string searchValue = data;
		CanvasElementsSearchClear();
		if (searchValue != "")
		{
			for (int i = 0; i < CanvasElements.size(); i++)
			{
				if (TextureLoader.GetTextureNameByIndex(CanvasElements[i].elementDataNumber).find(searchValue) != std::string::npos)
				{
					CanvasElements[i].isSearched = true;
				}
			}
		}
	}
#pragma endregion
#pragma region Helper functions for canvas (definition)
	void CanvasDragging(ImVec2 moveDelta)
	{
		//float reverseScale = ReverseScale(canvasScaleFactor);
		ImVec2 newOrigin = ImVec2(origin.x + moveDelta.x, origin.y + moveDelta.y);
		if (newOrigin.x > 0)
		{
			newOrigin.x = 0;
		}
		if (newOrigin.y > 0)
		{
			newOrigin.y = 0;
		}
		origin = newOrigin;
	}
	void CanvasScaling(float deltaValue, ImGuiIO& io)
	{
		/*if (canvasScaleFactor != canvasScaleFactor + (deltaValue * -0.1f))
		{
			std::cout << "test\n";
			ImVec2 distance = ImVec2(viewport->WorkSize.x / 2 - io.MousePos.x, viewport->WorkSize.y / 2 - io.MousePos.y);
			std::cout << distance.x;
			std::cout << "\n";
			std::cout << distance.y;
			std::cout << "\n";
			std::cout << "------------\n";
			CanvasDragging(distance);
			// get delta by multiply to canvas scale (canvas position)
			// divide this delta by 2 - movement delta for canvas when scale
		}*/
		canvasScaleFactor += deltaValue * -0.1f;
		if (canvasScaleFactor < 0.1f)
		{
			canvasScaleFactor = 0.1f;
		}
		else if (canvasScaleFactor > 2.0f)
		{
			canvasScaleFactor = 2.0f;
		}
		//std::cout << "wrong test\n";
	}
#pragma endregion
#pragma region Helper function for Rectangle Selection (definition)
	void CanvasRectangleSelection(ImGuiIO& io, ImVec2 SelectionStartPosition)
	{
		draw_list->AddRect(SelectionStartPosition, io.MousePos, IM_COL32(255, 255, 255, 255));
		draw_list->AddRectFilled(SelectionStartPosition, io.MousePos, IM_COL32(255, 255, 255, 15));
	}
	void SelectElementsInsideRectangle(ImGuiIO& io, ImVec2 start)
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (start.x < io.MousePos.x)
			{
				if ((CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor > start.x &&
					(CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor < io.MousePos.x)
				{
					if (start.y < io.MousePos.y)
					{
						if ((CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight > start.y &&
							(CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight < io.MousePos.y)
						{
							CanvasElements[i].isSelected = true;
							if (CanvasElements[i].isBlockSelection)
							{
								CanvasElements[i].isSelected = false;
							}
						}
					}
					else
					{
						if ((CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight < start.y &&
							(CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight > io.MousePos.y)
						{
							CanvasElements[i].isSelected = true;
							if (CanvasElements[i].isBlockSelection)
							{
								CanvasElements[i].isSelected = false;
							}
						}
					}
				}
			}
			else
			{
				if ((CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor < start.x &&
					(CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor > io.MousePos.x)
				{
					if (start.y < io.MousePos.y)
					{
						if ((CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight > start.y &&
							(CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight < io.MousePos.y)
						{
							CanvasElements[i].isSelected = true;
							if (CanvasElements[i].isBlockSelection)
							{
								CanvasElements[i].isSelected = false;
							}
						}
					}
					else
					{
						if ((CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight < start.y &&
							(CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight > io.MousePos.y)
						{
							CanvasElements[i].isSelected = true;
							if (CanvasElements[i].isBlockSelection)
							{
								CanvasElements[i].isSelected = false;
							}
						}
					}
				}
			}
		}
	}
	void UnselectElementsInsideRectangle(ImGuiIO& io, ImVec2 start)
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (start.x < io.MousePos.x)
			{
				if ((CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor <  start.x ||
					(CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor > io.MousePos.x)
				{
					CanvasElements[i].isSelected = false;
					if (CanvasElements[i].isBlockSelection)
					{
						CanvasElements[i].isSelected = true;
					}
				}
			}
			else
			{
				if ((CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor > start.x ||
					(CanvasElements[i].centerPosition.x + origin.x) * canvasScaleFactor < io.MousePos.x)
				{
					CanvasElements[i].isSelected = false;
					if (CanvasElements[i].isBlockSelection)
					{
						CanvasElements[i].isSelected = true;
					}
				}
			}
			if (start.y < io.MousePos.y)
			{
				if ((CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight < start.y ||
					(CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight > io.MousePos.y)
				{
					CanvasElements[i].isSelected = false;
					if (CanvasElements[i].isBlockSelection)
					{
						CanvasElements[i].isSelected = true;
					}
				}
			}
			else
			{
				if ((CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight > start.y ||
					(CanvasElements[i].centerPosition.y + origin.y) * canvasScaleFactor + topBarHeight < io.MousePos.y)
				{
					CanvasElements[i].isSelected = false;
					if (CanvasElements[i].isBlockSelection)
					{
						CanvasElements[i].isSelected = true;
					}
				}
			}
		}
	}
#pragma endregion
#pragma region Helper functions for Canvas Elements list (definition)
	void CanvasElementsSelectAll()
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			CanvasElements[i].isSelected = true;
		}
	}
	void ClearCanvasSelectedElementsAll()
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			CanvasElements[i].isSelected = false;
		}
	}
	void ResetCanvasSelectedElementsAll()
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (CanvasElements[i].isBlockSelection)
			{
				CanvasElements[i].isSelected = true;
				CanvasElements[i].isBlockSelection = false;
			}
			else
			{
				CanvasElements[i].isSelected = false;
			}
		}
	}
	void SetCanvasSelectedElementsBlockStatus(bool newValue = false)
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (newValue)
			{
				if (CanvasElements[i].isSelected)
				{
					CanvasElements[i].isBlockSelection = true;
				}
			}
			else
			{
				CanvasElements[i].isBlockSelection = newValue;
			}
		}
	}
	void CalculateSelectedCanvasElements()
	{
		selectedElementsCount = 0;
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (CanvasElements[i].isSelected)
			{
				selectedElementsCount++;
			}
		}
	}
	void CanvasElementDelete(int countOfDeleteOperation)
	{
		while (countOfDeleteOperation > 0)
		{
			for (int i = 0; i < CanvasElements.size(); i++)
			{
				if (CanvasElements[i].isSelected)
				{
					CanvasElements.erase(CanvasElements.begin() + i);
					countOfDeleteOperation--;
					if (CurrentHoveredElementIndex == i)
					{
						CurrentHoveredElementIndex = -1;
					}
					break;
				}
			}
		}

	}
	void CanvasElementsDragByValue(float xDelta, float yDelta)
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (CanvasElements[i].isSelected)
			{
				CanvasElements[i].position.x = CanvasElements[i].position.x + xDelta;
				CanvasElements[i].position.y = CanvasElements[i].position.y + yDelta;
				CanvasElements[i].centerPosition.x = CanvasElements[i].centerPosition.x + xDelta;
				CanvasElements[i].centerPosition.y = CanvasElements[i].centerPosition.y + yDelta;
				LinkRecalculate(&CanvasElements[i], ImVec2(xDelta, yDelta));
			}
		}
	}
	void CanvasElementsSearchClear()
	{
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			CanvasElements[i].isSearched = false;
		}
	}
#pragma endregion
	float ReverseScale(float value)
	{
		return abs(value - 2) + 0.1f;
		// 2 max
		// 0.1 min
		// 2 = 0.1
		// 0.1 = 2
		// > 1-
		// < 1

	}
#pragma region Debug (definition)
	std::string DEBUGstate(int index)
	{
		switch (index)
		{
		case 0: return "Rest";
		case 1: return "CanvasSelection";
		case 2: return "ElementSelection";
		case 3: return "ElementWindowSelection";
		case 4: return "CanvasMapWindowSelection";
		case 5: return "TopBarSelection";
		case 6: return "BottomBarSelection";
		case 7: return "ElementHover";
		case 8: return "RectangleSelection";
		case 9: return "RectangleSelectionPlus";
		case 10: return "ElementDrag";
		case 11: return "CanvasDrag";
		case 12: return "LinkPointHover";
		case 13: return "LinkCreate";
		case 14: return "LinkHover";
		}
		return "null";
	}
	void CanvasCenterRectangle()
	{
		draw_list->AddRect(ImVec2(viewport->WorkSize.x / 2 - 2.5f, viewport->WorkSize.y / 2 - 2.5f), ImVec2(viewport->WorkSize.x / 2 + 2.5f, viewport->WorkSize.y / 2 + 2.5f), IM_COL32(255, 255, 255, 255));
		draw_list->AddRectFilled(ImVec2(viewport->WorkSize.x / 2 - 2.5f, viewport->WorkSize.y / 2 - 2.5f), ImVec2(viewport->WorkSize.x / 2 + 2.5f, viewport->WorkSize.y / 2 + 2.5f), IM_COL32(255, 255, 255, 15));
	}
#pragma endregion
#pragma region Helper function for linking point location (definition)
	ImVec2 GetLinkingPointLocation(int Elem, int Point)
	{
		static const float Spacing = 0.25; // counted from texture height/width. (spacing = times from def. texture / height)
		// 0 = up linking point
		// 1 = right
		// 2 = down
		// 3 = left

		int Width = TextureLoader.GetTextureWidthByIndex(CanvasElements[Elem].elementDataNumber);
		int Height = TextureLoader.GetTextureHeightByIndex(CanvasElements[Elem].elementDataNumber);

		switch (Point)
		{
		case 0: return ImVec2(
			(origin.x + CanvasElements[Elem].position.x + Width / 2.0f) * canvasScaleFactor,
			topBarHeight + (origin.y + CanvasElements[Elem].position.y - Height * Spacing) * canvasScaleFactor
		); break;
		case 1: return ImVec2(
			(origin.x + CanvasElements[Elem].position.x + Width + Width * Spacing) * canvasScaleFactor,
			topBarHeight + (origin.y + CanvasElements[Elem].position.y + Height / 2.0f) * canvasScaleFactor
		); break;
		case 2: return ImVec2(
			(origin.x + CanvasElements[Elem].position.x + Width / 2.0f) * canvasScaleFactor,
			topBarHeight + (origin.y + CanvasElements[Elem].position.y + Height + Height * Spacing) * canvasScaleFactor
		); break;
		case 3: return ImVec2(
			(origin.x + CanvasElements[Elem].position.x - Width * Spacing) * canvasScaleFactor,
			topBarHeight + (origin.y + CanvasElements[Elem].position.y + Height / 2.0f) * canvasScaleFactor
		); break;
		}
		throw;
	}
#pragma endregion
#pragma region Helper functions for links (declaration)
	void CanvasAddLinkingPointsButton(int elem, int pin, const char* label, ImVec2 position)
	{
		static const float DotRadius = 5.0f;
		ImGui::SetCursorPos({ position.x - DotRadius, position.y - DotRadius - topBarHeight });
		ImGui::InvisibleButton(label, { DotRadius * 2.0f, DotRadius * 2.0f }, ImGuiButtonFlags_MouseButtonLeft);
		if (ImGui::IsItemHovered())
		{
			CurrentHoveredLinkingPointElementIndex = elem;
			CurrentHoveredLinkingPointPinIndex = pin;
			StateMachineSetState(LinkDotHover);
		}
	}
	void CanvasAddLinkingPointsButtons()
	{
		ImVec2 OldCursorPos = ImGui::GetCursorPos();
		for (int i = 0; i < CanvasElements.size(); i++)
		{
			if (CanvasElements[i].elementDataType & 1)
			{
				CanvasAddLinkingPointsButton(i, 0, (std::to_string(i) + "LinkingPointUp").c_str(), GetLinkingPointLocation(i, 0));
			}
			if (CanvasElements[i].elementDataType & 2)
			{
				CanvasAddLinkingPointsButton(i, 1, (std::to_string(i) + "LinkingPointRight").c_str(), GetLinkingPointLocation(i, 1));
			}
			if (CanvasElements[i].elementDataType & 4)
			{
				CanvasAddLinkingPointsButton(i, 2, (std::to_string(i) + "LinkingPointDown").c_str(), GetLinkingPointLocation(i, 2));
			}
			if (CanvasElements[i].elementDataType & 8)
			{
				CanvasAddLinkingPointsButton(i, 3, (std::to_string(i) + "LinkingPointLeft").c_str(), GetLinkingPointLocation(i, 3));
			}
		}
		ImGui::SetCursorPos(OldCursorPos);
	}
	void CanvasCreateLink(int first_element, int first_pin, int second_element, int second_pin)
	{
		CanvasLink NewLink;
		NewLink.first = &CanvasElements[first_element];
		NewLink.second = &CanvasElements[second_element];

		// linking dots - dummy code
		NewLink.linkDots.push_back(GetLinkingPointLocation(first_element, first_pin));
		NewLink.linkDots.push_back(GetLinkingPointLocation(second_element, second_pin));

		CanvasLinks.push_back(NewLink);
	}
	void CanvasDrawLineFromPointToMouse(ImVec2 point)
	{
		draw_list->AddLine(point, ImGui::GetMousePos(), IM_COL32(255, 255, 255, 255), 1.0f);
	}
#pragma endregion
	void LinkRecalculate(CanvasElement* element, ImVec2 delta)
	{
		for (int i = 0; i < CanvasLinks.size(); i++)
		{
			if (CanvasLinks[i].first == element)
			{
				CanvasLinks[i].linkDots[0].x += delta.x;
				CanvasLinks[i].linkDots[0].y += delta.y;
			}
			if (CanvasLinks[i].second == element)
			{
				CanvasLinks[i].linkDots[CanvasLinks[i].linkDots.size() - 1].x += delta.x;
				CanvasLinks[i].linkDots[CanvasLinks[i].linkDots.size() - 1].y += delta.y;
			}
		}
	}
}