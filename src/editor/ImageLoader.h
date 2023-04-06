#pragma once
#include <d3d11.h>

namespace EditorMMImageLoader
{
	bool LoadTextureFromFile(const char* filename, ID3D11ShaderResourceView** out_srv, int* out_width, int* out_height);
}