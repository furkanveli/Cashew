#include "D3DGraphics.h"

namespace Cashew
{
	using Microsoft::WRL::ComPtr;

	D3DGraphics::D3DGraphics(HWND hWnd, unsigned int width, unsigned int height)
		:m_hwnd(hWnd), m_width(width), m_height(height)
	{}

	void D3DGraphics::WaitForSignal()
	{
		m_currentFenceValue++;

		m_commandQueue->Signal(m_fence.Get(), m_currentFenceValue);

		if (m_fence->GetCompletedValue() < m_currentFenceValue)
		{
			HANDLE event = CreateEventW(nullptr, FALSE, FALSE, nullptr);
			if (!event)
			{
				throw ERR_LAST();
			}
			m_fence->SetEventOnCompletion(m_currentFenceValue, event) >> chk;

			WaitForSingleObject(event, INFINITE);
		}
	}

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

#endif

		const D3D12_COMMAND_QUEUE_DESC commandQueueDesc =
		{
			.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
			.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
			.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
			.NodeMask = 0
		};
	
		m_device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&m_commandQueue)) >> chk;

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
		ComPtr<IDXGISwapChain1> tempSwapChain;
		m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_hwnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain) >> chk;
		tempSwapChain.As(&m_swapChain) >> chk;

		const D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc =
		{
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV,
			.NumDescriptors = m_bufferCount
		};
		m_device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvDescriptorHeap)) >> chk;
		m_rtvDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
		
		CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		for (int i = 0; i < m_bufferCount; i++)
		{
			m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i])) >> chk;
			m_device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(m_rtvDescriptorSize);
		}

		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)) >> chk;
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)) >> chk;

		m_commandList->Close() >> chk;

		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) >> chk;

		return true;
	}

	void D3DGraphics::Render(const CashewTimer& timer)
	{
		// get current buffer index and get a reference to the back buffer. Reset the allocator and the list before we start
		m_curBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		auto& backBuffer = m_backBuffers[m_curBackBufferIndex];
		m_commandAllocator->Reset() >> chk;
		m_commandList->Reset(m_commandAllocator.Get(), nullptr);

		const auto presentToTarget = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &presentToTarget);

		FLOAT clearColor[] = { 0.4f, 0.9f, 0.7f, 1.0f };
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), (INT)m_curBackBufferIndex, m_rtvDescriptorSize);
		m_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);

		const auto TargetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &TargetToPresent);

		m_commandList->Close() >> chk;
		ID3D12CommandList* const commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		m_swapChain->Present(1, 0) >> chk;

		WaitForSignal();
	}

	
}