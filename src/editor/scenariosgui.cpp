// Dear ImGui: standalone example application for DirectX 11
// If you are new to Dear ImGui, read documentation from the docs/ folder + read the top of imgui.cpp.
// Read online: https://github.com/ocornut/imgui/tree/master/docs

#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#include <d3d11.h>
#include <tchar.h>
#include <iostream>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include <list>
#include <vector>

// Simple helper function to load an image into a DX11 texture with common settings


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
namespace ScenarioGUI {
	struct LoadedTextures
	{
		ID3D11ShaderResourceView* my_texture;
		int my_image_height;
		int my_image_width;
	};
	LoadedTextures TEMP; // В будущем - удалить
	LoadedTextures TEMP2; // В будущем - удалить
	// Data
	static ID3D11Device* g_pd3dDevice = NULL;
	static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	static IDXGISwapChain* g_pSwapChain = NULL;
	static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	// Forward declarations of helper functions
	void CreateDemoScenarioGUI();
	bool CreateDeviceD3D(HWND hWnd);
	void CleanupDeviceD3D();
	void CreateRenderTarget();
	void CleanupRenderTarget();
	void DrawObjects();
	void WorkspaceInitialization();
	void PopupWorkspace();
	void PopupElements();
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

	const ImGuiViewport* viewport;

	// Helper functions

	void CreateDemoScenarioGUI()
	{
		// Create application window
		//ImGui_ImplWin32_EnableDpiAwareness();
		WNDCLASSEXW wc = { sizeof(wc), CS_CLASSDC, WndProc, 0L, 0L, GetModuleHandle(NULL), NULL, NULL, NULL, NULL, L"ImGui Example", NULL };
		::RegisterClassExW(&wc);
		//HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"САПР КИТ", WS_OVERLAPPEDWINDOW, 100, 100, 1280, 800, NULL, NULL, wc.hInstance, NULL);
		HWND hwnd = ::CreateWindowW(wc.lpszClassName, L"САПР КИТ", WS_OVERLAPPEDWINDOW, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, wc.hInstance, NULL);

		// Initialize Direct3D
		if (!CreateDeviceD3D(hwnd))
		{
			CleanupDeviceD3D();
			::UnregisterClassW(wc.lpszClassName, wc.hInstance);
			//abort here
			// if directX not connected
		}

		// Show the window
		::ShowWindow(hwnd, SW_SHOWMAXIMIZED);
		::UpdateWindow(hwnd);

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;

		io.Fonts->AddFontFromFileTTF(u8"C:/Users/VR/Desktop/projects/SimulatorsEditor/src/editor/LiberationSans.ttf", 22.0f, NULL, io.Fonts->GetGlyphRangesCyrillic());
		//ImFont* font1 = io.Fonts->AddFontFromFileTTF("font.ttf", size_pixels);


		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
		//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

		// Setup Dear ImGui style
		//ImGui::StyleColorsDark();
		ImGui::StyleColorsLight();

		// Setup Platform/Renderer backends
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(g_pd3dDevice, g_pd3dDeviceContext);

		// Load Fonts
		// - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
		// - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
		// - If the file cannot be loaded, the function will return NULL. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
		// - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
		// - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
		// - Read 'docs/FONTS.md' for more instructions and details.
		// - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
		//io.Fonts->AddFontDefault();
		//io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
		//io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
		//ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, NULL, io.Fonts->GetGlyphRangesJapanese());
		//IM_ASSERT(font != NULL);
		ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
		// Main loop
		while (TEMP.my_image_height == 0)
		{
			if (bool ret = LoadTextureFromFile(u8"C:/Users/VR/Desktop/projects/SimulatorsEditor/src/editor/img/MyImage01.png", &TEMP.my_texture, &TEMP.my_image_width, &TEMP.my_image_height))
			{
				IM_ASSERT(ret);
			}
		}
		while (TEMP2.my_image_height == 0)
		{
			if (bool ret = LoadTextureFromFile(u8"C:/Users/VR/Desktop/projects/SimulatorsEditor/src/editor/img/MyImage02.png", &TEMP2.my_texture, &TEMP2.my_image_width, &TEMP2.my_image_height))
			{
				IM_ASSERT(ret);
			}
		}
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

			// Ниже - окна, отображаемые в редакторе сценариев

			// Меню

			ImGui::BeginMainMenuBar();
			if (ImGui::BeginMenu(u8"Файл"))
			{
				if (ImGui::MenuItem(u8"Открыть", "Ctrl+O")) {}
				if (ImGui::MenuItem(u8"Недавние файлы..."))
					//{
					//    //
					//    ImGui::EndMenu();
					//}
					if (ImGui::MenuItem(u8"Сохранить", "Ctrl+S")) {}
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

			// Область программы

			viewport = ImGui::GetMainViewport();
			WorkspaceInitialization();

			ImGui::SetNextWindowPos(viewport->WorkPos);
			ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, (viewport->WorkSize.y / 3) - 29));
			ImGui::Begin(u8"Сценарии", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImGui::End();

			ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, (viewport->WorkSize.y / 3) - 2));
			ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, viewport->WorkSize.y / 3));
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(15.0f, 15.0f)); // Отступы между кнопками
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 15));
			ImGui::Begin(u8"Элементы сценария", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			DrawObjects();
			ImGui::End();
			ImGui::PopStyleVar();
			ImGui::PopStyleVar();

			ImGui::SetNextWindowPos(ImVec2(viewport->WorkPos.x, (2 * viewport->WorkSize.y / 3) - 4));
			ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x / 4, (viewport->WorkSize.y / 3) + 33));
			ImGui::Begin(u8"Свойства", NULL, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize);
			ImGui::End();

			ImGui::ShowDemoWindow();

			// Конец окон

			// Rendering
			ImGui::Render();
			const float clear_color_with_alpha[4] = { clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w };
			g_pd3dDeviceContext->OMSetRenderTargets(1, &g_mainRenderTargetView, NULL);
			g_pd3dDeviceContext->ClearRenderTargetView(g_mainRenderTargetView, clear_color_with_alpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			g_pSwapChain->Present(1, 0); // Present with vsync
			//g_pSwapChain->Present(0, 0); // Present without vsync

		}


		// Cleanup
		ImGui_ImplDX11_Shutdown();
		ImGui_ImplWin32_Shutdown();
		ImGui::DestroyContext();

		CleanupDeviceD3D();
		::DestroyWindow(hwnd);
		::UnregisterClassW(wc.lpszClassName, wc.hInstance);

	}

	//Для сохранения элементов на рабочей панели
	struct ElementOnCanvas
	{
		const char* Path;
		ImVec2 Pos;
	};

	// Запоминаем расположенные элементы на поле, а также выбранные при выделении;
	static std::vector<ElementOnCanvas> Elems;
	static std::vector<int> SelectedElems;
	// Отображение рабочего места
	void WorkspaceInitialization()
	{
		ImGuiIO& io = ImGui::GetIO();
		// Задаем размеры
		ImGui::SetNextWindowPos(ImVec2(viewport->WorkSize.x / 4, viewport->WorkPos.y));
		ImGui::SetNextWindowSize(ImVec2(3 * viewport->WorkSize.x / 4, viewport->WorkSize.y));
		// Убираем отступы для корректного отображения
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("MainWorkspace", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);

		// Позиция + размеры рабочего поля
		static ImVec2 scrolling(0.0f, 0.0f); // Текущая прокрутка
		ImVec2 canvas_p0 = ImGui::GetCursorScreenPos();
		ImVec2 canvas_sz = ImGui::GetContentRegionAvail();
		ImVec2 canvas_p1 = ImVec2(canvas_p0.x + canvas_sz.x, canvas_p0.y + canvas_sz.y);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();
		
		//Используются для определения передвигаемого элемента
		static bool ClickedOnElement = false;
		static bool IsSelecting = false;
		static bool dragging = false;
		//Используются для определения места передвигаемого элемента
		static ImVec2 OldElementPosition;
		static ImVec2 SelectionStartPosition;
		draw_list->AddRectFilled(canvas_p0, canvas_p1, IM_COL32(240, 240, 240, 0));
		ImGui::InvisibleButton("canvas", canvas_sz, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight); // Невидимая кнопка для взаимодействия
		ImGui::SetItemAllowOverlap();
		bool WorkspaceHovered = ImGui::IsItemHovered();
		const bool is_active = ImGui::IsItemActive();
		// Прокрутка на ПКМ
		if (is_active && ImGui::IsMouseDragging(ImGuiMouseButton_Right) && !ClickedOnElement)
		{
			scrolling.x += io.MouseDelta.x;
			scrolling.y += io.MouseDelta.y;
			if (scrolling.x > 0.0f) scrolling.x = 0.0f;
			if (scrolling.y > 0.0f) scrolling.y = 0.0f;
		}
		// Рисуем сетку
		draw_list->PushClipRect(canvas_p0, canvas_p1, true);
		{
			const float GRID_STEP = 79.0f;
			for (float x = fmodf(scrolling.x, GRID_STEP); x < canvas_sz.x; x += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x + x, canvas_p0.y), ImVec2(canvas_p0.x + x, canvas_p1.y), IM_COL32(200, 200, 200, 119));
			for (float y = fmodf(scrolling.y, GRID_STEP); y < canvas_sz.y; y += GRID_STEP)
				draw_list->AddLine(ImVec2(canvas_p0.x, canvas_p0.y + y), ImVec2(canvas_p1.x, canvas_p0.y + y), IM_COL32(200, 200, 200, 119));
		}
		draw_list->PopClipRect();
		// Возвращаем старый стиль отступов
		ImGui::PopStyleVar();
		// Вызов контекстного меню
		ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
		if (drag_delta.x == 0.0f && drag_delta.y == 0.0f)
			ImGui::OpenPopupOnItemClick("contextworkspace", ImGuiPopupFlags_MouseButtonRight);
		if (ImGui::BeginPopup("contextworkspace"))
		{
			PopupWorkspace();
			ImGui::EndPopup();
		}
		// Отслеживание позиции поля с учетом прокрутки
		const ImVec2 origin(canvas_p0.x + scrolling.x, canvas_p0.y + scrolling.y);
		const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
		// Приемник drag'n'drop
		if (ImGui::BeginDragDropTarget()) {

			auto payload = ImGui::AcceptDragDropPayload("Element");
			if (payload != NULL) {
				int ElementNum = *(int*)payload->Data;
				const char* Name;
				Name = ElementNum % 2 == 1 ? u8"MyImage01.png" : u8"MyImage02.png";
				int x = mouse_pos_in_canvas.x - TEMP.my_image_width / 2;
				int y = mouse_pos_in_canvas.y - TEMP.my_image_height / 2;
				if (x < 0) x = 0;
				if (y < 0) y = 0;
				Elems.push_back({ Name, ImVec2(x, y) });
			}
			ImGui::EndDragDropTarget();
		}
		// Отображаем добавленные элементы
		ElementOnCanvas ToAdd;
		// Решение ниже - для того, чтобы при нажатии выбирался верхний элемент. При использовании IsItemHovered() выбираются все элементы, попадающие под курсор. Уверен, есть более рациональное решение, пока не нашел
		// https://github.com/ocornut/imgui/issues/3909 - работает криво
		// Переносим объект - удаляем его из списка, добавляем в конец
		// IsItemActive - перехватывает лишь при нажатии ЛКМ и ПКМ
		if (IsSelecting) SelectedElems.clear();
		for (int i = 0; i < static_cast<int>(Elems.size()); i++)
		{
			// Уйдет на замену
			if (Elems[i].Path == "MyImage01.png")
				draw_list->AddImage((void*)TEMP.my_texture, ImVec2(origin.x + Elems[i].Pos.x, origin.y + Elems[i].Pos.y), ImVec2(origin.x + Elems[i].Pos.x + TEMP.my_image_width, origin.y + Elems[i].Pos.y + TEMP.my_image_height));
			else if (Elems[i].Path == "MyImage02.png")
				draw_list->AddImage((void*)TEMP2.my_texture, ImVec2(origin.x + Elems[i].Pos.x, origin.y + Elems[i].Pos.y), ImVec2(origin.x + Elems[i].Pos.x + TEMP2.my_image_width, origin.y + Elems[i].Pos.y + TEMP2.my_image_height));
			// Конец замены
			// Создаем невидимую кнопку для взаимодействия с объектом на рабочем пространстве
			ImGui::SetCursorScreenPos(ImVec2(origin.x + Elems[i].Pos.x, origin.y + Elems[i].Pos.y));
			ImGui::InvisibleButton("canvas123", ImVec2(TEMP.my_image_width, TEMP.my_image_height), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			ImGui::SetItemAllowOverlap();
			// Контексное меню
			ImVec2 drag_delta = ImGui::GetMouseDragDelta(ImGuiMouseButton_Right);
			if (drag_delta.x == 0.0f && drag_delta.y == 0.0f)
				ImGui::OpenPopupOnItemClick("contextelem", ImGuiPopupFlags_MouseButtonRight);
			if (ImGui::BeginPopup("contextelem"))
			{
				PopupElements();
				ImGui::EndPopup();
			}
			// Нажатия на кнопку обрабатываются криво, поэтому используется связка IsMouseClicked() + IsItemHovered(), возможно, изменить позже
			// Поиск элемента, лежащего "сверху"
			if (ImGui::IsItemHovered())
			{
				// Нажатие на элемент - перерисовка элемента "поверх", запоминание его при передвижении (вылет, если курсор покинет невидимую кнопку)

				// Если не идет множественное выделение, единственный элемент считаем выбранныс
				if (!IsSelecting && ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					// Убеждаемся, что элемент уже не был выбран
					if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
					{
						SelectedElems.clear();
						SelectedElems.push_back(i);
					}
					ClickedOnElement = true;
					OldElementPosition = Elems[i].Pos;
				}
				// Уже нажали, переносим
				if (ImGui::IsMouseDragging(ImGuiMouseButton_Left) && !IsSelecting)
				{
					dragging = true;
				}
				WorkspaceHovered = false;
			}
			// Если идет выбор - убеждаемся, что элемент попадает в выделение, добавляем в список выбранных элементов
			if (IsSelecting)
				if (Elems[i].Pos.x + origin.x > (io.MousePos.x + SelectionStartPosition.x - max(io.MousePos.x, SelectionStartPosition.x)) && Elems[i].Pos.x + origin.x < (io.MousePos.x + SelectionStartPosition.x - min(io.MousePos.x, SelectionStartPosition.x)))
					if (Elems[i].Pos.y + origin.y > (io.MousePos.y + SelectionStartPosition.y - max(io.MousePos.y, SelectionStartPosition.y)) && Elems[i].Pos.y + origin.y < (io.MousePos.y + SelectionStartPosition.y - min(io.MousePos.y, SelectionStartPosition.y)))
					{
						if (std::find(SelectedElems.begin(), SelectedElems.end(), i) == SelectedElems.end())
						SelectedElems.push_back(i);
					}
		}
		
		// Нажали на поле - сброс выделения
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ClickedOnElement && WorkspaceHovered)
		{
			IsSelecting = true;
			SelectionStartPosition = io.MousePos;
			SelectedElems.clear();
		}
		// Отрисовка окна выделения
		if (IsSelecting)
		{
			draw_list->AddRect(SelectionStartPosition, io.MousePos, IM_COL32(0, 0, 0, 255));
			draw_list->AddRectFilled(SelectionStartPosition, io.MousePos, IM_COL32(0, 0, 0, 15));
		} 
		// Реализует отрисовку передвигаемого/нажатого элемента поверх других, а также перенос элементов
		int j = static_cast<int>(SelectedElems.size());
		for (int i = 0; i < j; i++)
		{
			int SelectedElem = SelectedElems[i];
			// Визуальное отображение того, что выделяется
			draw_list->AddRect(ImVec2(origin.x + Elems[SelectedElem].Pos.x, origin.y + Elems[SelectedElem].Pos.y), ImVec2(origin.x + Elems[SelectedElem].Pos.x + TEMP.my_image_width, origin.y + Elems[SelectedElem].Pos.y + TEMP.my_image_height), IM_COL32(0, 0, 0, 255));
			draw_list->AddRectFilled(ImVec2(origin.x + Elems[SelectedElem].Pos.x, origin.y + Elems[SelectedElem].Pos.y), ImVec2(origin.x + Elems[SelectedElem].Pos.x + TEMP.my_image_width, origin.y + Elems[SelectedElem].Pos.y + TEMP.my_image_height), IM_COL32(255, 255, 0, 10));
			// Перенос не требует отрисовки "поверх", лишь смену позиции
			if (dragging)
			{
				int x = Elems[SelectedElem].Pos.x + io.MouseDelta.x;
				int y = Elems[SelectedElem].Pos.y + io.MouseDelta.y;
				if (x < 0 || io.MousePos.x < origin.x) x = 0;
				if (y < 0 || io.MousePos.y < origin.y) y = 0;
				Elems[SelectedElem].Pos = ImVec2(x, y);
			}
			// При нажатии на край элемента, лежащего за другим, требуется перерисовка "поверх"
			else if (ClickedOnElement && !dragging && j < 2)
			{
				ToAdd = { Elems[SelectedElem].Path, Elems[SelectedElem].Pos };
				Elems.erase(Elems.begin() + SelectedElem);
				Elems.push_back(ToAdd);
				SelectedElems[i] = -1;
				SelectedElems.push_back(static_cast<int>(Elems.size()) - 1 - i);
			}
		}

		auto new_end = std::remove(SelectedElems.begin(), SelectedElems.end(), -1);
		SelectedElems.erase(new_end, SelectedElems.end());
		if (!io.MouseDown[0])
		{
			dragging = ClickedOnElement = IsSelecting = false;
		}
		ImGui::End();

	}

	void PopupWorkspace()
	{
		if (ImGui::MenuItem("Remove all12312312312312312312312312312312312312312", NULL, false)) {}
	}

	void PopupElements()
	{
		if (ImGui::MenuItem("Remove sdfsdf", NULL, false)) {}
	}
	
	// Отображение элементов сценария
	void DrawObjects()
	{
		// Ниже - временное решение
		for (int i = 0; i <= 15; i++)
		{
			LoadedTextures TEMP = i % 2 == 1 ? ScenarioGUI::TEMP : TEMP2;
			ImGui::SameLine(); // Для актуальности GetCursorPos()
			// Рассчитываем, требуется ли перенос
			if (ImGui::GetStyle().ItemSpacing.x + TEMP.my_image_width > ImGui::GetContentRegionAvail().x)
			{
				ImGui::NewLine();
				ImGui::SetCursorPos(ImVec2(ImGui::GetCursorPos().x + ImGui::GetStyle().ItemSpacing.x, ImGui::GetCursorPos().y)); // Отступы второй и далее строки
			}
			ImGui::PushID(i); // Иначе - кнопки нагружены кривым Payload'ом (вызов BeginDragDropSource() происходит для всех элементов одновременно. Разные строки - вылет
			if (ImGui::ImageButton("Element", (void*)TEMP.my_texture, ImVec2(TEMP.my_image_width, TEMP.my_image_height)));
			// Источник drag'n'drop
			if (ImGui::BeginDragDropSource())
			{
				ImGui::SetDragDropPayload("Element", &i, sizeof(int), ImGuiCond_Once);
				ImGui::Text("Payload data is: %d", *(int*)ImGui::GetDragDropPayload()->Data);
				ImGui::EndDragDropSource();
			}
			ImGui::PopID();
		}
		// Конец временного решения
	}

	// Ниже - вспомогательные функции 
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height)
	{
		// Load from disk into a raw RGBA buffer
		int image_width = 0;
		int image_height = 0;
		unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
		if (image_data == NULL)
			return false;

		// Create texture
		D3D11_TEXTURE2D_DESC desc;
		ZeroMemory(&desc, sizeof(desc));
		desc.Width = image_width;
		desc.Height = image_height;
		desc.MipLevels = 1;
		desc.ArraySize = 1;
		desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		desc.SampleDesc.Count = 1;
		desc.Usage = D3D11_USAGE_DEFAULT;
		desc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		desc.CPUAccessFlags = 0;

		ID3D11Texture2D* pTexture = NULL;
		D3D11_SUBRESOURCE_DATA subResource;
		subResource.pSysMem = image_data;
		subResource.SysMemPitch = desc.Width * 4;
		subResource.SysMemSlicePitch = 0;
		g_pd3dDevice->CreateTexture2D(&desc, &subResource, &pTexture);

		// Create texture view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		ZeroMemory(&srvDesc, sizeof(srvDesc));
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = desc.MipLevels;
		srvDesc.Texture2D.MostDetailedMip = 0;
		g_pd3dDevice->CreateShaderResourceView(pTexture, &srvDesc, out_srv);
		pTexture->Release();

		*out_width = image_width;
		*out_height = image_height;
		stbi_image_free(image_data);

		return true;
	}

	bool CreateDeviceD3D(HWND hWnd)
	{
		// Setup swap chain
		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 2;
		sd.BufferDesc.Width = 0;
		sd.BufferDesc.Height = 0;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hWnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = true;
		sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

		UINT createDeviceFlags = 0;
		//createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
		D3D_FEATURE_LEVEL featureLevel;
		const D3D_FEATURE_LEVEL featureLevelArray[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
		if (D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, createDeviceFlags, featureLevelArray, 2, D3D11_SDK_VERSION, &sd, &g_pSwapChain, &g_pd3dDevice, &featureLevel, &g_pd3dDeviceContext) != S_OK)
			return false;

		CreateRenderTarget();
		return true;
	}

	void CleanupDeviceD3D()
	{
		CleanupRenderTarget();
		if (g_pSwapChain) { g_pSwapChain->Release(); g_pSwapChain = NULL; }
		if (g_pd3dDeviceContext) { g_pd3dDeviceContext->Release(); g_pd3dDeviceContext = NULL; }
		if (g_pd3dDevice) { g_pd3dDevice->Release(); g_pd3dDevice = NULL; }
	}

	void CreateRenderTarget()
	{
		ID3D11Texture2D* pBackBuffer;
		g_pSwapChain->GetBuffer(0, IID_PPV_ARGS(&pBackBuffer));
		g_pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &g_mainRenderTargetView);
		pBackBuffer->Release();
	}

	void CleanupRenderTarget()
	{
		if (g_mainRenderTargetView) { g_mainRenderTargetView->Release(); g_mainRenderTargetView = NULL; }
	}

	LRESULT WINAPI WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		// Win32 message handler
		// You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
		// - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
		// - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
		// Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;

		switch (msg)
		{
		case WM_SIZE:
			if (g_pd3dDevice != NULL && wParam != SIZE_MINIMIZED)
			{
				CleanupRenderTarget();
				g_pSwapChain->ResizeBuffers(0, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
				CreateRenderTarget();
			}
			return 0;
		case WM_SYSCOMMAND:
			if ((wParam & 0xfff0) == SC_KEYMENU) // Disable ALT application menu
				return 0;
			break;
		case WM_DESTROY:
			::PostQuitMessage(0);
			return 0;
		}
		return ::DefWindowProc(hWnd, msg, wParam, lParam);
	}
}