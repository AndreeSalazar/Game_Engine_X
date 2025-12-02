#pragma once

#include "D3D12Core.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace D3D12Core {

    class D3D12DescriptorHeap {
    public:
        D3D12DescriptorHeap();
        ~D3D12DescriptorHeap();

        bool Initialize(
            ID3D12Device* device,
            D3D12_DESCRIPTOR_HEAP_TYPE type,
            UINT numDescriptors,
            bool shaderVisible = false
        );
        void Shutdown();

        ID3D12DescriptorHeap* GetHeap() const { return m_heap.Get(); }
        D3D12_CPU_DESCRIPTOR_HANDLE GetCPUHandle(UINT index) const;
        D3D12_GPU_DESCRIPTOR_HANDLE GetGPUHandle(UINT index) const;
        UINT GetDescriptorSize() const { return m_descriptorSize; }
        UINT GetCapacity() const { return m_capacity; }

    private:
        ComPtr<ID3D12DescriptorHeap> m_heap;
        D3D12_CPU_DESCRIPTOR_HANDLE m_cpuStartHandle = {};
        D3D12_GPU_DESCRIPTOR_HANDLE m_gpuStartHandle = {};
        UINT m_descriptorSize = 0;
        UINT m_capacity = 0;
        D3D12_DESCRIPTOR_HEAP_TYPE m_type;
    };

} // namespace D3D12Core

