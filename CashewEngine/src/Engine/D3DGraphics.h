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
#include "Log.h"
#include "CashewError.h"
#include "CashewTimer.h"


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

	
		// D3D12 API convenience variables
	private:
		static const UINT m_bufferCount = 2;
		DXGI_FORMAT m_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		UINT m_rtvDescriptorSize = 0;
		uint64_t m_currentFenceValue = 0;
		UINT m_curBackBufferIndex = 0;

		// D3D12 API member variables
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_device;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_backBuffers[m_bufferCount];
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		
		// D3D12 API member functions
	private:
		void WaitForSignal();
	public:
		bool Init();
		void Render(const CashewTimer& timer);
	};
}