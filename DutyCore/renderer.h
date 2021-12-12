#pragma once

#include <d3d11.h>
#include <dxgi.h>

namespace dutycore
{
	class renderer
	{
		using tPresent = HRESULT(*)(IDXGISwapChain*, ID3D11Device**, ID3D11DeviceContext**);
		static tPresent oPresent;

		static bool initializedImgui;

		static void CleanupDXDevice();
		static HRESULT GetDXDeviceAndCtxFromSwapchain(IDXGISwapChain* chain, ID3D11Device** device, ID3D11DeviceContext** deviceCtx);
		static HRESULT OnPresent(IDXGISwapChain* chain, ID3D11Device** device, ID3D11DeviceContext** deviceCtx);

	public:
		static void SetupDXDevice();
	};
}
