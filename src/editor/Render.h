#pragma once

namespace EditorMMRender 
{
	//static ID3D11Device* g_pd3dDevice = NULL;
	//static ID3D11DeviceContext* g_pd3dDeviceContext = NULL;
	//static IDXGISwapChain* g_pSwapChain = NULL;
	//static ID3D11RenderTargetView* g_mainRenderTargetView = NULL;

	void Prepare();
	void Cleanup();
	void Render();

	/*class RenderD3D11Data
	{
	public:
		static ID3D11Device* g_pd3dDevice;
		static ID3D11DeviceContext* g_pd3dDeviceContext;
		static IDXGISwapChain* g_pSwapChain;
		static ID3D11RenderTargetView* g_mainRenderTargetView;
	};*/
}