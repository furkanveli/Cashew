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

	Microsoft::WRL::ComPtr<ID3DBlob> D3DGraphics::CompileShader(const std::wstring& filename, const const D3D_SHADER_MACRO* defines, const std::string& entrypoint, const std::string& target)
	{
		UINT compileFlags = 0;
#ifdef CASHEW_DEBUG
		compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif
		HRESULT hr = S_OK;

		ComPtr<ID3DBlob> byteCode = nullptr;
		ComPtr<ID3DBlob> errors;
		hr = D3DCompileFromFile(filename.c_str(), defines, D3D_COMPILE_STANDARD_FILE_INCLUDE,
			entrypoint.c_str(), target.c_str(), compileFlags, 0, &byteCode, &errors);

		if (errors != nullptr)
			ENGINE_TRACE("{}", (char*)errors->GetBufferPointer());

		hr >> chk;

		return byteCode;
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
		// vertex data
		UINT nVertices;
		const Vertex vertexData[] = {
			{ { 0.00f,  0.50f, 0.0f}, {1.0f, 0.0f, 0.0f}},
			{ { 0.50f, -0.25f, 0.0f}, {0.0f, 0.0f, 1.0f}},
			{ { -0.5f, -0.25f, 0.0f}, {0.0f, 1.0f, 0.0f}}
		};

		nVertices = (UINT)std::size(vertexData);
		// vertex buffer and vertex upload buffer
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(vertexData));
		m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_COPY_DEST, nullptr, IID_PPV_ARGS(&m_vertexBuffer)) >> chk;

		const CD3DX12_HEAP_PROPERTIES heapProps2{ D3D12_HEAP_TYPE_UPLOAD };
		m_device->CreateCommittedResource(&heapProps2, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_vertexUploadBuffer)) >> chk;

		Vertex* mappedVertexData = nullptr;
		m_vertexUploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData)) >> chk;
		memcpy(mappedVertexData, vertexData, sizeof(vertexData));
		m_vertexUploadBuffer->Unmap(0, nullptr);

		m_commandAllocator->Reset() >> chk;
		m_commandList->Reset(m_commandAllocator.Get(), nullptr) >> chk;
		m_commandList->CopyResource(m_vertexBuffer.Get(), m_vertexUploadBuffer.Get());
		m_commandList->Close() >> chk;
		ID3D12CommandList* const commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		WaitForSignal();

		// vertex buffer view
		m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vertexBufferView.SizeInBytes = nVertices * sizeof(Vertex);
		m_vertexBufferView.StrideInBytes = sizeof(Vertex);

		// root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		if (const auto hr = D3D12SerializeRootSignature(
			&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob); FAILED(hr))
		{
			if (errorBlob)
			{
				auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
				//ENGINE_TRACE("{}", ToWstring(errorBufferPtr));
			}
			hr >> chk;
		}
		
		m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)) >> chk;

		WCHAR workingDir[MAX_PATH];
		GetCurrentDirectoryW(MAX_PATH, workingDir);
		ENGINE_TRACE("{}", ToAString(workingDir));

		ComPtr<ID3DBlob> vBlob;
		D3DReadFileToBlob(L"../CashewEngine/src/Engine/Shaders/ShadersCompiled/VertexShader.cso", &vBlob) >> chk;
		ComPtr<ID3DBlob> pBlob;
		D3DReadFileToBlob(L"../CashewEngine/src/Engine/Shaders/ShadersCompiled/PixelShader.cso", &pBlob) >> chk;
		// pipelinestate
		struct PipelineStateStream
		{
			CD3DX12_PIPELINE_STATE_STREAM_ROOT_SIGNATURE RootSignature;
			CD3DX12_PIPELINE_STATE_STREAM_INPUT_LAYOUT InputLayout;
			CD3DX12_PIPELINE_STATE_STREAM_PRIMITIVE_TOPOLOGY PrimitiveTopologyType;
			CD3DX12_PIPELINE_STATE_STREAM_VS VS;
			CD3DX12_PIPELINE_STATE_STREAM_PS PS;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
		}pipelineStateStream;

		const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};
	
		pipelineStateStream.RootSignature = m_rootSignature.Get();
		pipelineStateStream.InputLayout = { inputLayout, (UINT)std::size(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vBlob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pBlob.Get());
		pipelineStateStream.RTVFormats = { .RTFormats{DXGI_FORMAT_R8G8B8A8_UNORM}, .NumRenderTargets = 1 };

		const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(PipelineStateStream), &pipelineStateStream };
		m_device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pipelineState)) >> chk;

		

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

		m_commandList->SetPipelineState(m_pipelineState.Get());
		m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);

		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);

		m_commandList->OMSetRenderTargets(1, &rtv, TRUE, nullptr);
		m_commandList->DrawInstanced(3, 1, 0, 0);
		//** present and end **
		const auto TargetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &TargetToPresent);

		m_commandList->Close() >> chk;
		ID3D12CommandList* const commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		m_swapChain->Present(1, 0) >> chk;
		//** present and end **

		WaitForSignal();
	}

	
}