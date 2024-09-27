#include "D3DGraphics.h"

namespace Cashew
{
	using Microsoft::WRL::ComPtr;

	D3DGraphics::D3DGraphics(HWND hWnd, unsigned int width, unsigned int height)
		:m_hwnd(hWnd), m_width(width), m_height(height)
	{}

	bool D3DGraphics::Init()
	{
#ifdef CASHEW_DEBUG
		ComPtr<ID3D12Debug4> debugController;
		D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)) >> chk;
		debugController->EnableDebugLayer();
		debugController->SetEnableGPUBasedValidation(true);

		
#endif
		CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&m_dxgiFactory)) >> chk;
		D3D12CreateDevice(nullptr, D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&m_device)) >> chk;

#ifdef CASHEW_DEBUG
		// info queue for console debug messages
		QueueInit(m_device.Get());
		D3D12InfoQueue->ClearStoredMessages();
#endif

		const D3D12_COMMAND_QUEUE_DESC desc =
		{
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};
	
		m_device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_commandQueue)) >> chk;

		const DXGI_SWAP_CHAIN_DESC1 swapChainDesc =
		{
			.Width = m_width,
			.Height = m_height,
			.Format = m_backBufferFormat,
			.Stereo = FALSE,
			.SampleDesc =
			{
				.Count = 1,
				.Quality = 0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = m_bufferCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = 0,
		};
		m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_hwnd, &swapChainDesc, nullptr, nullptr, &m_swapChain) >> chk;
		
		return true;
	}

	
}