#pragma once
#include <windows.h>
#include <d3d12.h>
#include "d3dx12.h"
#include <dxgi1_6.h>
#include <d3dcompiler.h>
#include "d3d12sdklayers.h"
#include "dxgidebug.h"
#include <DirectXMath.h>
#include <wrl.h>
#include <variant>
#include "Macros.h"
#include "Log.h"
#include "CashewError.h"
#include "CashewTimer.h"
#include "Window.h"
#include "ConstantBuffer.h"

namespace Cashew
{ 
	
	class CASHEW_API D3DGraphics
	{
	private:
		struct Vertex
		{
			DirectX::XMFLOAT3 position;
			DirectX::XMFLOAT4 color;
		};
		enum class BufferType
		{
			Vertex,
			Index
		};


	public:
		D3DGraphics(HWND hWnd,  unsigned int width, unsigned int height);
		D3DGraphics(const D3DGraphics& gfx) = delete;
		D3DGraphics& operator=(const D3DGraphics& gfx) = delete;
		
	private:
		HWND m_hwnd;
		unsigned int m_width;
		unsigned int m_height;

	
		// D3D12 API convenience variables
	private:
		static const UINT m_bufferCount = 2;
		DXGI_FORMAT m_frameBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_FORMAT m_depthBufferFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;
		UINT m_numCbvDescriptors = 1;
		UINT m_rtvDescriptorSize = 0;
		UINT m_CbvSrvUavDescriptorSize = 0;
		uint64_t m_currentFenceValue = 0;
		UINT m_curFrameBufferIndex = 0;
		float aspectRatio = static_cast<float>(m_width) / static_cast<float>(m_height);
		
		std::vector<Vertex> m_vertexArray;
		std::vector<WORD> m_indexArray;
		std::vector<DirectX::XMFLOAT4X4> m_mvpArray;

		DirectX::XMFLOAT4X4 m_view;
		DirectX::XMFLOAT4X4 m_projection;
		
		
		// D3D12 API member variables
	private:
		Microsoft::WRL::ComPtr<IDXGIFactory4> m_dxgiFactory;
		Microsoft::WRL::ComPtr<ID3D12Device2> m_device;
		Microsoft::WRL::ComPtr<IDXGISwapChain3> m_swapChain;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_commandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_commandAllocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_rtvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_dsvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_cbvDescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_frameBuffers[m_bufferCount];
		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_depthBuffer;
		ConstantBuffer<DirectX::XMFLOAT4X4> m_mvpBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView;
		D3D12_INDEX_BUFFER_VIEW m_indexBufferView;
		CD3DX12_RECT m_scissorRect{ 0, 0, LONG_MAX, LONG_MAX};
		CD3DX12_VIEWPORT m_viewport{ 0.0f, 0.0f, float(m_width), float(m_height) };
		// D3D12 API member functions
	private:
		void WaitForSignal();
		void Init();
		void SetupDevice();
		void SetupGeometry();
		void CreateDescriptorHeaps();
		template <typename T>
		void CreateBuffer(Microsoft::WRL::ComPtr<ID3D12Resource>& buffer, std::vector<T> data, BufferType type);
		template <typename T>
		std::variant<D3D12_VERTEX_BUFFER_VIEW, D3D12_INDEX_BUFFER_VIEW> CreateBufferView(Microsoft::WRL::ComPtr<ID3D12Resource>& buffer, std::vector<T> data, BufferType type);
		void CreateRootAndPipeline();
		void LogHardware(); 
	public:
		void Render(const CashewTimer& timer, Keyboard& kbd, Mouse& mouse);

		unsigned int GetWidth();
		unsigned int GetHeight();
		void SetWidth(unsigned int w);
		void SetHeight(unsigned int h);
		bool DeviceExists();
		void OnResize();
	};
}