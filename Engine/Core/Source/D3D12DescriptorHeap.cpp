#include "D3D12DescriptorHeap.h"
#include <iostream>

namespace D3D12Core {

    D3D12DescriptorHeap::D3D12DescriptorHeap() {
    }

    D3D12DescriptorHeap::~D3D12DescriptorHeap() {
        Shutdown();
    }

    bool D3D12DescriptorHeap::Initialize(
        ID3D12Device* device,
        D3D12_DESCRIPTOR_HEAP_TYPE type,
        UINT numDescriptors,
        bool shaderVisible
    ) {
        m_type = type;
        m_capacity = numDescriptors;

        D3D12_DESCRIPTOR_HEAP_DESC desc = {};
        desc.Type = type;
        desc.NumDescriptors = numDescriptors;
        desc.Flags = shaderVisible ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        HRESULT hr = device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_heap));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create descriptor heap" << std::endl;
            return false;
        }

        m_cpuStartHandle = m_heap->GetCPUDescriptorHandleForHeapStart();
        if (shaderVisible) {
            m_gpuStartHandle = m_heap->GetGPUDescriptorHandleForHeapStart();
        }

        m_descriptorSize = device->GetDescriptorHandleIncrementSize(type);

        return true;
    }

    void D3D12DescriptorHeap::Shutdown() {
        m_heap.Reset();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetCPUHandle(UINT index) const {
        D3D12_CPU_DESCRIPTOR_HANDLE handle = m_cpuStartHandle;
        handle.ptr += index * m_descriptorSize;
        return handle;
    }

    D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetGPUHandle(UINT index) const {
        D3D12_GPU_DESCRIPTOR_HANDLE handle = m_gpuStartHandle;
        handle.ptr += index * m_descriptorSize;
        return handle;
    }

} // namespace D3D12Core

