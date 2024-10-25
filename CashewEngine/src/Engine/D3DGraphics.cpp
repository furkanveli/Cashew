#include "Cashewpch.h"

#include "D3DGraphics.h"

namespace Cashew
{
	using Microsoft::WRL::ComPtr;
	using namespace DirectX;

	D3DGraphics::D3DGraphics(HWND hWnd, unsigned int width, unsigned int height)
		:m_hwnd(hWnd), m_width(width), m_height(height)
	{
		Init();
	}

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

	void D3DGraphics::SetupDevice()
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
			.Format = m_frameBufferFormat,
			.Stereo = FALSE,
			.SampleDesc =
			{
				1,
				0
			},
			.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
			.BufferCount = m_bufferCount,
			.Scaling = DXGI_SCALING_STRETCH,
			.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
			.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
			.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
		};
		ComPtr<IDXGISwapChain1> tempSwapChain;
		m_dxgiFactory->CreateSwapChainForHwnd(m_commandQueue.Get(), m_hwnd, &swapChainDesc, nullptr, nullptr, &tempSwapChain) >> chk;
		tempSwapChain.As(&m_swapChain) >> chk;

		m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_commandAllocator)) >> chk;
		m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_commandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_commandList)) >> chk;

		m_commandList->Close() >> chk;

		m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence)) >> chk;

		
	}

	void D3DGraphics::SetupGeometry()
	{
		m_vertexArray = {
			Vertex({ XMFLOAT3(-1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::White) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Black) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, -1.0f), XMFLOAT4(Colors::Red) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, -1.0f), XMFLOAT4(Colors::Green) }),
			Vertex({ XMFLOAT3(-1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Blue) }),
			Vertex({ XMFLOAT3(-1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Yellow) }),
			Vertex({ XMFLOAT3(+1.0f, +1.0f, +1.0f), XMFLOAT4(Colors::Cyan) }),
			Vertex({ XMFLOAT3(+1.0f, -1.0f, +1.0f), XMFLOAT4(Colors::Magenta) })
		};
		m_indexArray = {
			// front face
		0, 1, 2,
		0, 2, 3,

		// back face
		4, 6, 5,
		4, 7, 6,

		// left face
		4, 5, 1,
		4, 1, 0,

		// right face
		3, 2, 6,
		3, 6, 7,

		// top face
		1, 5, 6,
		1, 6, 2,

		// bottom face
		4, 0, 3,
		4, 3, 7
		};
		const auto eyePosition = XMVectorSet(0, 0, 5, 1);
		const auto focusPoint = XMVectorSet(0, 0, 0, 1);
		const auto upDirection = XMVectorSet(0, 1, 0, 0);
		XMStoreFloat4x4(&m_view, XMMatrixLookAtLH(eyePosition, focusPoint, upDirection));
		XMStoreFloat4x4(&m_projection, XMMatrixPerspectiveFovLH(XMConvertToRadians(90.f), aspectRatio, 0.1f, 100.0f));
		XMFLOAT4X4 mvp;
		XMStoreFloat4x4(&mvp, XMLoadFloat4x4(&m_projection) * XMLoadFloat4x4(&m_view));
		m_mvpArray.push_back(mvp);
		
	}
	void D3DGraphics::CreateDescriptorHeaps()
	{
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
			m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_frameBuffers[i])) >> chk;
			m_device->CreateRenderTargetView(m_frameBuffers[i].Get(), nullptr, rtvHandle);
			rtvHandle.Offset(m_rtvDescriptorSize);
		}

		const CD3DX12_HEAP_PROPERTIES depthHeapProp(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC depthDesc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R24G8_TYPELESS,
			m_width, m_height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
		D3D12_CLEAR_VALUE clearValue = {
			.Format = m_depthBufferFormat,
			.DepthStencil = {1.0f, 0}
		};
		D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
		dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
		dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
		dsvDesc.Format = m_depthBufferFormat;
		dsvDesc.Texture2D.MipSlice = 0;

		m_device->CreateCommittedResource(&depthHeapProp, D3D12_HEAP_FLAG_NONE, &depthDesc, D3D12_RESOURCE_STATE_COMMON, &clearValue, IID_PPV_ARGS(&m_depthBuffer)) >> chk;

		const D3D12_DESCRIPTOR_HEAP_DESC dsvHeapDesc = {
			.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV,
			.NumDescriptors = 1
		};
		m_device->CreateDescriptorHeap(&dsvHeapDesc, IID_PPV_ARGS(&m_dsvDescriptorHeap)) >> chk;

		const CD3DX12_CPU_DESCRIPTOR_HANDLE dsvHandle{ m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart() };
		m_device->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, dsvHandle);

		m_commandAllocator->Reset() >> chk;
		m_commandList->Reset(m_commandAllocator.Get(), nullptr) >> chk;
		auto commonToWrite = CD3DX12_RESOURCE_BARRIER::Transition(m_depthBuffer.Get(),
			D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
		m_commandList->ResourceBarrier(1, &commonToWrite);

		m_commandList->Close() >> chk;
		ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

		WaitForSignal();
		m_CbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		D3D12_DESCRIPTOR_HEAP_DESC cbvHeapDesc;
		cbvHeapDesc.NumDescriptors = m_numCbvDescriptors;
		cbvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
		cbvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		cbvHeapDesc.NodeMask = 0;
		m_device->CreateDescriptorHeap(&cbvHeapDesc, IID_PPV_ARGS(&m_cbvDescriptorHeap));
		m_CbvSrvUavDescriptorSize = m_device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
	}
	template <typename T>
	void D3DGraphics::CreateBuffer(Microsoft::WRL::ComPtr<ID3D12Resource>& buffer, std::vector<T> data, BufferType type)
	{
		
			auto dest = D3D12_RESOURCE_STATES(0);
			if (type == BufferType::Vertex)
				dest = D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER;
			if (type == BufferType::Index)
				dest = D3D12_RESOURCE_STATE_INDEX_BUFFER;
			// vertex buffer and vertex upload buffer
			const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_DEFAULT };
			const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(sizeof(T) * data.size());
			m_device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATES(0), nullptr, IID_PPV_ARGS(buffer.GetAddressOf())) >> chk;

			Microsoft::WRL::ComPtr<ID3D12Resource> UploadBuffer;
			const CD3DX12_HEAP_PROPERTIES heapProps2{ D3D12_HEAP_TYPE_UPLOAD };
			m_device->CreateCommittedResource(&heapProps2, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATES(0), nullptr, IID_PPV_ARGS(&UploadBuffer)) >> chk;

			const auto copyDest = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_COPY_DEST);
			const auto backcopyDest = CD3DX12_RESOURCE_BARRIER::Transition(buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, dest);
			const auto genRead = CD3DX12_RESOURCE_BARRIER::Transition(UploadBuffer.Get(), D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_GENERIC_READ);
			const auto backgenRead = CD3DX12_RESOURCE_BARRIER::Transition(UploadBuffer.Get(), D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_STATE_COMMON);
			D3D12_RESOURCE_BARRIER barriers[] = { copyDest, genRead };
			D3D12_RESOURCE_BARRIER backbarriers[] = { backcopyDest, backgenRead };

			Vertex* mappedVertexData = nullptr;
			UploadBuffer->Map(0, nullptr, reinterpret_cast<void**>(&mappedVertexData)) >> chk;
			memcpy(mappedVertexData, data.data(), sizeof(T) * data.size());
			UploadBuffer->Unmap(0, nullptr);

			m_commandAllocator->Reset() >> chk;
			m_commandList->Reset(m_commandAllocator.Get(), nullptr) >> chk;

			m_commandList->ResourceBarrier(sizeof(barriers) / sizeof(D3D12_RESOURCE_BARRIER), barriers);
			m_commandList->CopyResource(buffer.Get(), UploadBuffer.Get());
			m_commandList->ResourceBarrier(sizeof(backbarriers) / sizeof(D3D12_RESOURCE_BARRIER), backbarriers);
			m_commandList->Close() >> chk;

			ID3D12CommandList* const commandLists[] = { m_commandList.Get() };
			m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

			WaitForSignal();
		

	}
	template <typename T>
	std::variant<D3D12_VERTEX_BUFFER_VIEW, D3D12_INDEX_BUFFER_VIEW> D3DGraphics::CreateBufferView(Microsoft::WRL::ComPtr<ID3D12Resource>& buffer, std::vector<T> data, BufferType type)
	{
		if (type == BufferType::Vertex)
		{
			D3D12_VERTEX_BUFFER_VIEW bufferView;
			bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
			bufferView.SizeInBytes = static_cast<UINT>(data.size()) * sizeof(T);
			bufferView.StrideInBytes = sizeof(T);
			return bufferView;
		}
		else if (type == BufferType::Index)
		{
			D3D12_INDEX_BUFFER_VIEW bufferView;
			bufferView.BufferLocation = buffer->GetGPUVirtualAddress();
			bufferView.SizeInBytes = static_cast<UINT>(data.size()) * sizeof(T);
			bufferView.Format = DXGI_FORMAT_R16_UINT;
			return bufferView;
		}
	}

	void D3DGraphics::CreateRootAndPipeline()
	{
		//define root parameter for mvp
		CD3DX12_ROOT_PARAMETER rootParameters[1]{};
		CD3DX12_DESCRIPTOR_RANGE cbvTable{};
		cbvTable.Init(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, 1, 0); // view matrix 
		rootParameters[0].InitAsDescriptorTable(1, &cbvTable);
		// root signature
		CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
		rootSignatureDesc.Init((UINT)std::size(rootParameters), rootParameters, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);
		ComPtr<ID3DBlob> signatureBlob;
		ComPtr<ID3DBlob> errorBlob;
		if (const auto hr = D3D12SerializeRootSignature(
			&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob); FAILED(hr))
		{
			if (errorBlob)
			{
				auto errorBufferPtr = static_cast<const char*>(errorBlob->GetBufferPointer());
				ENGINE_TRACE("{}", errorBufferPtr);
			}
			hr >> chk;
		}

		m_device->CreateRootSignature(0, signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(), IID_PPV_ARGS(&m_rootSignature)) >> chk;

		// Load shaders
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
			CD3DX12_PIPELINE_STATE_STREAM_DEPTH_STENCIL_FORMAT DSVFormat;
			CD3DX12_PIPELINE_STATE_STREAM_RENDER_TARGET_FORMATS RTVFormats;
			CD3DX12_PIPELINE_STATE_STREAM_RASTERIZER Rasterizer;
		}pipelineStateStream;
		

		const D3D12_INPUT_ELEMENT_DESC inputLayout[] =
		{
			{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0},
			{"COLOR"   , 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0}
		};

		CD3DX12_RASTERIZER_DESC rasterizerDesc(D3D12_DEFAULT);

		pipelineStateStream.RootSignature = m_rootSignature.Get();
		pipelineStateStream.InputLayout = { inputLayout, (UINT)std::size(inputLayout) };
		pipelineStateStream.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		pipelineStateStream.VS = CD3DX12_SHADER_BYTECODE(vBlob.Get());
		pipelineStateStream.PS = CD3DX12_SHADER_BYTECODE(pBlob.Get());
		pipelineStateStream.DSVFormat = m_depthBufferFormat;
		pipelineStateStream.RTVFormats = { .RTFormats{DXGI_FORMAT_R8G8B8A8_UNORM}, .NumRenderTargets = 1 };
		pipelineStateStream.Rasterizer = rasterizerDesc;

		const D3D12_PIPELINE_STATE_STREAM_DESC pipelineStateStreamDesc = { sizeof(PipelineStateStream), &pipelineStateStream };
		m_device->CreatePipelineState(&pipelineStateStreamDesc, IID_PPV_ARGS(&m_pipelineState)) >> chk;
	}

	void D3DGraphics::LogHardware()
	{
		UINT i = 0;
		IDXGIAdapter* adapter = nullptr;
		std::vector<IDXGIAdapter*> adapterList;
		while (m_dxgiFactory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND)
		{
			DXGI_ADAPTER_DESC desc;
			adapter->GetDesc(&desc);

			std::wstring text = L"---Adapter: ";
			text += desc.Description;
			text += L"\n";

			ENGINE_TRACE("{}", ToAString(text.c_str()));
			
			adapterList.emplace_back(adapter);

			++i;
		}

		for (size_t i = 0; i < adapterList.size(); ++i)
		{
			UINT y = 0;
			IDXGIOutput* output = nullptr;
			while (adapterList[i]->EnumOutputs(y, &output) != DXGI_ERROR_NOT_FOUND)
			{
				DXGI_OUTPUT_DESC desc;
				output->GetDesc(&desc);

				std::wstring text = L"---Output: ";
				text += desc.DeviceName;
				text += L"\n";
				ENGINE_TRACE("{}", ToAString(text.c_str()));

				UINT count = 0;
				UINT flags = 0;

				output->GetDisplayModeList(m_frameBufferFormat, flags, &count, nullptr);

				std::vector<DXGI_MODE_DESC> modeList(count);
				output->GetDisplayModeList(m_frameBufferFormat, flags, &count, &modeList[0]);

				for (auto& x : modeList)
				{
					UINT n = x.RefreshRate.Numerator;
					UINT d = x.RefreshRate.Denominator;
					std::wstring text = 
						L"Width = " + std::to_wstring(x.Width) + L" " +
						L"Height = " + std::to_wstring(x.Height) + L" " +
						L"Refresh = " + std::to_wstring(n) + L"/" + std::to_wstring(d) +
						L"\n";
					ENGINE_TRACE("{}", ToAString(text.c_str()));
				}

				if (output)
				{
					output->Release();
					output = 0;
				}

				++i;
			}
			if (adapterList[i])
			{
				adapterList[i]->Release();
				adapterList[i] = 0;
			}
		}
	}


	void D3DGraphics::Init()
	{
		SetupDevice();
		CreateDescriptorHeaps();
		SetupGeometry();
		CreateBuffer(m_vertexBuffer, m_vertexArray, BufferType::Vertex);
		CreateBuffer(m_indexBuffer , m_indexArray , BufferType::Index);
		m_mvpBuffer.Init(m_device.Get(), m_mvpArray.size());
		m_mvpBuffer.AddView(m_device.Get(), m_cbvDescriptorHeap.Get(), m_mvpArray.size(), 0); // attach the constantbuffer to the descriptor heap
		
		
		// vertex buffer view
		m_vertexBufferView = std::get<D3D12_VERTEX_BUFFER_VIEW>(CreateBufferView(m_vertexBuffer, m_vertexArray, BufferType::Vertex));
		m_indexBufferView = std::get<D3D12_INDEX_BUFFER_VIEW>(CreateBufferView(m_indexBuffer, m_indexArray, BufferType::Index));
		CreateRootAndPipeline();
	}

	
	void D3DGraphics::Render(const CashewTimer& timer, Keyboard& kbd,  Mouse& mouse)
	{
		static float t = 0;

		m_curFrameBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
		auto& backBuffer = m_frameBuffers[m_curFrameBufferIndex];
		m_commandAllocator->Reset() >> chk;
		m_commandList->Reset(m_commandAllocator.Get(),nullptr);

		const auto presentToTarget = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		m_commandList->ResourceBarrier(1, &presentToTarget);

		FLOAT clearColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
		const CD3DX12_CPU_DESCRIPTOR_HANDLE rtv(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(), (INT)m_curFrameBufferIndex, m_rtvDescriptorSize);
		const CD3DX12_CPU_DESCRIPTOR_HANDLE dsv(m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
		m_commandList->ClearRenderTargetView(rtv, clearColor, 0, nullptr);
		m_commandList->ClearDepthStencilView(dsv, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		m_commandList->SetPipelineState(m_pipelineState.Get());
		m_commandList->SetGraphicsRootSignature(m_rootSignature.Get());
		m_commandList->SetDescriptorHeaps(1, m_cbvDescriptorHeap.GetAddressOf());

		m_commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		m_commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
		m_commandList->IASetIndexBuffer(&m_indexBufferView);
		m_commandList->RSSetViewports(1, &m_viewport);
		m_commandList->RSSetScissorRects(1, &m_scissorRect);

		m_commandList->OMSetRenderTargets(1, &rtv, TRUE, &dsv);
		

		auto translate1 = XMMatrixRotationZ(t) * XMMatrixRotationX(t);
		auto mvp1 = XMMatrixTranspose(translate1 * XMLoadFloat4x4(&m_view) * XMLoadFloat4x4(&m_projection));
		XMFLOAT4X4 storedmvp1;
		XMStoreFloat4x4(&storedmvp1, mvp1);
		m_mvpBuffer.CopyData(0, storedmvp1);
		

		const CD3DX12_GPU_DESCRIPTOR_HANDLE cbvview(m_cbvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());
		m_commandList->SetGraphicsRootDescriptorTable(0, cbvview);
		m_commandList->DrawIndexedInstanced(m_indexArray.size(), 1, 0, 0, 0);
		//** present and end **


		const auto TargetToPresent = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer.Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		m_commandList->ResourceBarrier(1, &TargetToPresent);

		m_commandList->Close() >> chk;
		ID3D12CommandList* const commandLists[] = { m_commandList.Get() };
		m_commandQueue->ExecuteCommandLists(_countof(commandLists), commandLists);

		m_swapChain->Present(1, 0) >> chk;
		//** present and end **
		t = t + 0.06;
		WaitForSignal();
	}

	unsigned int D3DGraphics::GetWidth()
	{
		return m_width;
	}

	unsigned int D3DGraphics::GetHeight()
	{
		return m_height;
	}
	void D3DGraphics::SetWidth(unsigned int w)
	{
		m_width = w;
	}
	void D3DGraphics::SetHeight(unsigned int h)
	{
		m_height = h;
	}
	bool D3DGraphics::DeviceExists()
	{
		return m_device;
	}
	void D3DGraphics::OnResize()
	{
		if (m_device && m_swapChain && m_commandAllocator)
		{
			WaitForSignal();

			m_commandList->Reset(m_commandAllocator.Get(), nullptr) >> chk;
			for (int i = 0; i < m_bufferCount; ++i)
			{
				m_frameBuffers[i].Reset();
			}
			m_depthBuffer.Reset();

			m_swapChain->ResizeBuffers(m_bufferCount, m_width, m_height, m_frameBufferFormat, DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH) >> chk;

			m_curFrameBufferIndex = 0;
			CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHeapHandle(m_rtvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());
			for (UINT i = 0; i < m_bufferCount; i++)
			{
				m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_frameBuffers[i])) >> chk;
				m_device->CreateRenderTargetView(m_frameBuffers[i].Get(), nullptr, rtvHeapHandle);
				rtvHeapHandle.Offset(1, m_rtvDescriptorSize);
			}
			D3D12_RESOURCE_DESC depthStencilDesc;
			depthStencilDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
			depthStencilDesc.Alignment = 0;
			depthStencilDesc.Width = m_width;
			depthStencilDesc.Height = m_height;
			depthStencilDesc.DepthOrArraySize = 1;
			depthStencilDesc.MipLevels = 1;
			depthStencilDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
			depthStencilDesc.SampleDesc.Count = 1;
			depthStencilDesc.SampleDesc.Quality = 0;
			depthStencilDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
			depthStencilDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

			auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
			D3D12_CLEAR_VALUE optClear;
			optClear.Format = m_depthBufferFormat;
			optClear.DepthStencil.Depth = 1.0f;
			optClear.DepthStencil.Stencil = 0;
			m_device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&depthStencilDesc,
				D3D12_RESOURCE_STATE_COMMON,
				&optClear,
				IID_PPV_ARGS(m_depthBuffer.GetAddressOf()));

			D3D12_DEPTH_STENCIL_VIEW_DESC dsvDesc;
			dsvDesc.Flags = D3D12_DSV_FLAG_NONE;
			dsvDesc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
			dsvDesc.Format = m_depthBufferFormat;
			dsvDesc.Texture2D.MipSlice = 0;
			m_device->CreateDepthStencilView(m_depthBuffer.Get(), &dsvDesc, m_dsvDescriptorHeap->GetCPUDescriptorHandleForHeapStart());

			// Transition the resource from its initial state to be used as a depth buffer.
			auto commonToWrite = CD3DX12_RESOURCE_BARRIER::Transition(m_depthBuffer.Get(),
				D3D12_RESOURCE_STATE_COMMON, D3D12_RESOURCE_STATE_DEPTH_WRITE);
			m_commandList->ResourceBarrier(1, &commonToWrite);

			m_commandList->Close() >> chk;
			ID3D12CommandList* cmdLists[] = { m_commandList.Get() };
			m_commandQueue->ExecuteCommandLists(_countof(cmdLists), cmdLists);

			m_viewport.Width = static_cast<float>(m_width);
			m_viewport.Height = static_cast<float>(m_height);

			WaitForSignal();
		}
	}

}