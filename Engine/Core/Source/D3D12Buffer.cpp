#include "D3D12Buffer.h"
#include <iostream>

namespace D3D12Core {

    D3D12Buffer::D3D12Buffer() {
    }

    D3D12Buffer::~D3D12Buffer() {
        Shutdown();
    }

    bool D3D12Buffer::Initialize(
        ID3D12Device* device,
        UINT64 size,
        D3D12_RESOURCE_FLAGS flags,
        D3D12_RESOURCE_STATES initialState
    ) {
        m_size = size;
        // Los buffers siempre se crean en COMMON, independientemente del initialState especificado
        // Guardamos el estado deseado para después de la copia
        m_currentState = initialState;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC resourceDesc = {};
        resourceDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        resourceDesc.Alignment = 0;
        resourceDesc.Width = size;
        resourceDesc.Height = 1;
        resourceDesc.DepthOrArraySize = 1;
        resourceDesc.MipLevels = 1;
        resourceDesc.Format = DXGI_FORMAT_UNKNOWN;
        resourceDesc.SampleDesc.Count = 1;
        resourceDesc.SampleDesc.Quality = 0;
        resourceDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        resourceDesc.Flags = flags;

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &resourceDesc,
            initialState,
            nullptr,
            IID_PPV_ARGS(&m_resource)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create buffer resource" << std::endl;
            return false;
        }

        return true;
    }

    void D3D12Buffer::Shutdown() {
        m_uploadBuffer.Reset();
        m_resource.Reset();
    }

    D3D12_GPU_VIRTUAL_ADDRESS D3D12Buffer::GetGPUVirtualAddress() const {
        return m_resource ? m_resource->GetGPUVirtualAddress() : 0;
    }

    bool D3D12Buffer::UploadData(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const void* data, UINT64 size) {
        if (size > m_size) {
            std::cerr << "Error: Data size exceeds buffer size" << std::endl;
            return false;
        }

        // Crear upload buffer
        D3D12_HEAP_PROPERTIES uploadHeapProps = {};
        uploadHeapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        uploadHeapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        uploadHeapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC uploadDesc = {};
        uploadDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        uploadDesc.Width = size;
        uploadDesc.Height = 1;
        uploadDesc.DepthOrArraySize = 1;
        uploadDesc.MipLevels = 1;
        uploadDesc.Format = DXGI_FORMAT_UNKNOWN;
        uploadDesc.SampleDesc.Count = 1;
        uploadDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;

        HRESULT hr = device->CreateCommittedResource(
            &uploadHeapProps,
            D3D12_HEAP_FLAG_NONE,
            &uploadDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_uploadBuffer)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create upload buffer" << std::endl;
            return false;
        }

        // Copiar datos al upload buffer
        void* mappedData = nullptr;
        D3D12_RANGE readRange = { 0, 0 };
        hr = m_uploadBuffer->Map(0, &readRange, &mappedData);
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to map upload buffer" << std::endl;
            return false;
        }

        memcpy(mappedData, data, size);
        m_uploadBuffer->Unmap(0, nullptr);

        // Los buffers siempre se crean en estado COMMON, no importa el initialState especificado
        // Transición del buffer destino a COPY_DEST solo si no está ya en ese estado
        if (m_currentState != D3D12_RESOURCE_STATE_COPY_DEST) {
            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = m_resource.Get();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COMMON; // Los buffers siempre empiezan en COMMON
            barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_DEST;
            commandList->ResourceBarrier(1, &barrier);
        }

        // Copiar datos
        commandList->CopyBufferRegion(m_resource.Get(), 0, m_uploadBuffer.Get(), 0, size);

        // Transición de vuelta al estado deseado solo si es diferente a COPY_DEST
        if (m_currentState != D3D12_RESOURCE_STATE_COPY_DEST) {
            D3D12_RESOURCE_BARRIER barrier = {};
            barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
            barrier.Transition.pResource = m_resource.Get();
            barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_DEST;
            barrier.Transition.StateAfter = m_currentState;
            commandList->ResourceBarrier(1, &barrier);
        }

        return true;
    }

} // namespace D3D12Core

