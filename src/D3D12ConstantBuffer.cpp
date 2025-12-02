#include "../include/D3D12ConstantBuffer.h"
#include "../include/D3D12Core.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <iostream>
#include <cstring>

using Microsoft::WRL::ComPtr;

namespace D3D12Core {

    D3D12ConstantBuffer::D3D12ConstantBuffer() : m_mappedData(nullptr), m_size(0) {
    }

    D3D12ConstantBuffer::~D3D12ConstantBuffer() {
        Shutdown();
    }

    bool D3D12ConstantBuffer::Initialize(ID3D12Device* device, UINT size) {
        m_size = size;
        
        // Alinear a 256 bytes (requerimiento de DirectX 12)
        UINT64 alignedSize = (size + 255) & ~255;

        // Crear buffer upload heap para constant buffer
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Width = alignedSize;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        ComPtr<ID3D12Resource> uploadBuffer;
        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&uploadBuffer)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create constant buffer" << std::endl;
            return false;
        }

        // Mapear el buffer
        D3D12_RANGE readRange = { 0, 0 };
        hr = uploadBuffer->Map(0, &readRange, &m_mappedData);
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to map constant buffer" << std::endl;
            return false;
        }

        // Guardar el resource directamente
        m_resource = uploadBuffer;

        return true;
    }

    void D3D12ConstantBuffer::Shutdown() {
        if (m_resource && m_mappedData) {
            m_resource->Unmap(0, nullptr);
            m_mappedData = nullptr;
        }
        m_resource.Reset();
    }

    void D3D12ConstantBuffer::UpdateData(const void* data, UINT size) {
        if (m_mappedData && size <= m_size) {
            memcpy(m_mappedData, data, size);
        }
    }

    void D3D12ConstantBuffer::Bind(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex) {
        if (m_resource) {
            D3D12_GPU_VIRTUAL_ADDRESS gpuAddress = m_resource->GetGPUVirtualAddress();
            commandList->SetGraphicsRootConstantBufferView(rootParameterIndex, gpuAddress);
        }
    }

    D3D12_GPU_VIRTUAL_ADDRESS D3D12ConstantBuffer::GetGPUVirtualAddress() const {
        if (m_resource) {
            return m_resource->GetGPUVirtualAddress();
        }
        return 0;
    }

} // namespace D3D12Core

