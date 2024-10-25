#pragma once
#include "Macros.h"
#include "wrl/client.h"
#include <d3d12.h>
#include "CashewError.h"
namespace Cashew
{
	template<typename T>
	class  ConstantBuffer
	{
	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_constantBuffer;
		BYTE* m_mappedData = nullptr;
		UINT m_elementByteSize;
	public:
		ConstantBuffer() = default;
		ConstantBuffer(const ConstantBuffer& c) = delete;
		ConstantBuffer operator=(const ConstantBuffer& c) = delete;
		~ConstantBuffer();
		void Init(ID3D12Device* device, UINT elementCount);
		ID3D12Resource* Get() const;
		void CopyData(int elementIndex, const T& data);
		void CopyDataArray(int elementIndex, const T*, size_t count);
		void AddView(ID3D12Device* device, ID3D12DescriptorHeap* heap, UINT elementCount, UINT slot);
	};


	template<typename T>
	void ConstantBuffer<T>::Init(ID3D12Device* device, UINT elementCount)
	{
		m_elementByteSize = (sizeof(T) + 255) & ~255;
		auto heapProps = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
		auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(m_elementByteSize * elementCount);
		device->CreateCommittedResource(&heapProps, D3D12_HEAP_FLAG_NONE, &resourceDesc, D3D12_RESOURCE_STATE_GENERIC_READ, nullptr, IID_PPV_ARGS(&m_constantBuffer)) >> chk;
		m_constantBuffer->Map(0, nullptr, reinterpret_cast<void**>(&m_mappedData));

	}
	template<typename T>
	ConstantBuffer<T>::~ConstantBuffer()
	{
		if (m_constantBuffer != nullptr)
			m_constantBuffer->Unmap(0, nullptr);
		m_mappedData = nullptr;
	}
	template<typename T>
	ID3D12Resource* ConstantBuffer<T>::Get() const
	{
		return m_constantBuffer.Get();
	}
	template<typename T>
	void ConstantBuffer<T>::CopyData(int elementIndex, const T& data)
	{
		memcpy(&m_mappedData[elementIndex * m_elementByteSize], &data, sizeof(T));
	}

	template<typename T>
	inline void ConstantBuffer<T>::CopyDataArray(int elementIndex, const T* data, size_t count)
	{
		memcpy(&m_mappedData[elementIndex * m_elementByteSize], data, sizeof(T) * count);
	}

	template<typename T>
	inline void ConstantBuffer<T>::AddView(ID3D12Device* device, ID3D12DescriptorHeap* heap, UINT elementCount, UINT slot)
	{
		D3D12_GPU_VIRTUAL_ADDRESS cbAddress = m_constantBuffer->GetGPUVirtualAddress();
		D3D12_CONSTANT_BUFFER_VIEW_DESC cbvDesc;
		cbvDesc.BufferLocation = cbAddress;
		cbvDesc.SizeInBytes = m_elementByteSize * elementCount;
		if(slot == 0)
			device->CreateConstantBufferView(&cbvDesc, heap->GetCPUDescriptorHandleForHeapStart());
		else
		{
			UINT offsetSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
			CD3DX12_CPU_DESCRIPTOR_HANDLE cbvHandle(heap->GetCPUDescriptorHandleForHeapStart());
			cbvHandle.Offset(1, offsetSize);
			device->CreateConstantBufferView(&cbvDesc, cbvHandle);
		}
	}
	
}