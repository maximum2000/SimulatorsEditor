#pragma once

#include <d3d11.h>

namespace EditorMMRender
{
	void Prepare();
	void Cleanup();
	void Render();
	void CreateTexture(D3D11_TEXTURE2D_DESC* desc, D3D11_SUBRESOURCE_DATA* subResource, ID3D11Texture2D** pTexture);
	void CreateTextureView(ID3D11Texture2D* pTexture, D3D11_SHADER_RESOURCE_VIEW_DESC* srvDesc, ID3D11ShaderResourceView** out_srv);
}