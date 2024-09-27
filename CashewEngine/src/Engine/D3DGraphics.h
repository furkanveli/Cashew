#pragma once
#include <windows.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3d12sdklayers.h"
#include "dxgidebug.h"
#include <wrl.h>
#include "Macros.h"
#include "CashewError.h"


namespace Cashew
{ 
	
	class CASHEW_API D3DGraphics
	{
	public:
		D3DGraphics(HWND hWnd,  unsigned int, unsigned int height);
		D3DGraphics(const D3DGraphics& gfx) = delete;
		D3DGraphics& operator=(const D3DGraphics& gfx) = delete;
		
	private:
		HWND m_hwnd;
		unsigned int m_width;
		unsigned int m_height;

		// D3D12 API member variables
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<IDXGISwapChain1> m_swapChain;


		// D3D12 API convenience variables
	private:
		const UINT m_bufferCount = 2;
		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

		// D3D12 API member functions
	public:
		bool Init();
	};
}