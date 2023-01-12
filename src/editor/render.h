//
// Window, directx11, win32, ImGUI render-related functions
//

#pragma once

namespace ScenariosEditorRender
{
	// Initializing and erasing functions
	void InitializeWindowAndImGUIContext();
	void EraseWindowAndImGuiContext();

	// In-loop render-related functions
	void StartFrame();
	bool HandleMessages();
	void EndFrame();

	// texture loading
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
}