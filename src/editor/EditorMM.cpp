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

namespace EditorMathModel
{
	// Forward declarations of helper functions
	void CreateDemoScenarioGUI();
    void ShowEditorScreen();
    void DrawTopBar(bool show_main_screen, bool show);
    void DrawElementsWindow(bool show);
    void DrawCanvas();
    void CanvasDrarElements(ImGuiIO& io);

    // Forward declarations of variables
    bool show_elements_window = false;

    static ImVec2 MousePos, origin;

    // ImGui data
    const ImGuiViewport* viewport;
    ImDrawList* draw_list;

    // Forward declarations of classes
    EditorMMTextureLoader::TextureLoader TextureLoader;










    struct CanvasElement
    {
        int Element;
        ImVec2 Pos;
        //int Type;
    };

    static std::vector<CanvasElement> CanvasElements;









    void CreateDemoScenarioGUI()
    {

        EditorMMRender::Prepare();

        // Our state
        bool show_demo_window = true;
        bool show_another_window = false;
        bool show = true;

        bool show_main_screen = true;

        TextureLoader.LoadToList();

        viewport = ImGui::GetMainViewport();

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
            ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);
            // 2. Show a simple window that we create ourselves. We use a Begin/End pair to create a named window.
            {
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
            }
            if (show_elements_window)
            {
                DrawElementsWindow(show);
            }
            DrawCanvas();
            DrawTopBar(show_main_screen, show);
            // Rendering
            EditorMMRender::Render();
        }
        EditorMMRender::Cleanup();
    }

    void ShowEditorScreen() 
    {

    }

    void DrawElementsWindow(bool show)
    {
        ImGui::SetNextWindowSize(ImVec2(400, 400), 0);
        ImGui::Begin("Elements", &show, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse);
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
        ImGui::End();
    }

    // draw top menu bar
    void DrawTopBar(bool show_main_screen, bool show)
    {
        if (show_main_screen)
        {
            ImGui::SetNextWindowPos(ImVec2(0, 0), 0);
            ImGui::SetNextWindowSize(ImGui::GetIO().DisplaySize, 0);
            ImGui::Begin("Main Window", &show, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoMove |
                ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoBringToFrontOnFocus);
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
                    if (ImGui::MenuItem("Export as")) {}
                    ImGui::Separator();
                    if (ImGui::MenuItem("Exit")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Edit"))
                {
                    if (ImGui::MenuItem("Undo", "Ctrl+Z")) {}
                    if (ImGui::MenuItem("Redo", "Ctrl+Y")) {}
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Window"))
                {
                    if (ImGui::MenuItem("Elements window"))
                    {
                        show_elements_window = !show_elements_window;
                    }
                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Options"))
                {
                    ImGui::EndMenu();
                }
                ImGui::EndMenuBar();
            }
            ImGui::End();
        }
    }

    void DrawCanvas() 
    {
        ImGuiIO& io = ImGui::GetIO();
        // Canvas size and style
        ImGui::SetNextWindowPos(ImVec2(0, 20)); // CHANGE 20 TO DYMANIC
        ImGui::SetNextWindowSize(ImVec2(viewport->WorkSize.x, viewport->WorkSize.y));
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
        ImGui::Begin("MainWorkspace", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoBringToFrontOnFocus);
        ImGui::PopStyleVar();
        // Canvas positioning
        static ImVec2 scrolling(0.0f, 0.0f); // current scrolling
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

        const ImVec2 mouse_pos_in_canvas(io.MousePos.x - origin.x, io.MousePos.y - origin.y);
        MousePos = mouse_pos_in_canvas;

        // canvas is drag'n'drop reciever
        if (ImGui::BeginDragDropTarget()) 
        {

            auto payload = ImGui::AcceptDragDropPayload("Element");
            if (payload != NULL) 
            {
                int ElementNum = *(int*)payload->Data;
                int x = mouse_pos_in_canvas.x - TextureLoader.GetTextureByIndex(ElementNum).imageWidth / 2;
                int y = mouse_pos_in_canvas.y - TextureLoader.GetTextureByIndex(ElementNum).imageHeight / 2;
                if (x < 0) x = 0;
                if (y < 0) y = 0;
                CanvasElements.push_back({ ElementNum, ImVec2(x, y) });
            }
            ImGui::EndDragDropTarget();
        }

        CanvasDrarElements(io);
    }

    void CanvasDrarElements(ImGuiIO& io) 
    {
        for (int i = 0; i < CanvasElements.size(); i++)
        {
            EditorMMTextureLoader::LoadedTexture UsedTexture = TextureLoader.GetTextureByIndex(CanvasElements[i].Element);
            draw_list->AddImage((void*)UsedTexture.myTexture, ImVec2(origin.x + CanvasElements[i].Pos.x, origin.y + CanvasElements[i].Pos.y), ImVec2(origin.x + CanvasElements[i].Pos.x + UsedTexture.imageWidth, origin.y + CanvasElements[i].Pos.y + UsedTexture.imageHeight));
            ImGui::SetCursorScreenPos(ImVec2(origin.x + CanvasElements[i].Pos.x, origin.y + CanvasElements[i].Pos.y));
            ImGui::InvisibleButton("canvas123", ImVec2(UsedTexture.imageWidth, UsedTexture.imageHeight), ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
            ImGui::SetItemAllowOverlap();
        }
    }
}