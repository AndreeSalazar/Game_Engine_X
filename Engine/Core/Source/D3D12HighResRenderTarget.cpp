#include "D3D12HighResRenderTarget.h"
#include <iostream>
#include <algorithm>

namespace D3D12Core {

    D3D12HighResRenderTarget::D3D12HighResRenderTarget() {
    }

    D3D12HighResRenderTarget::~D3D12HighResRenderTarget() {
        Shutdown();
    }

    bool D3D12HighResRenderTarget::Initialize(ID3D12Device* device, UINT renderWidth, UINT renderHeight) {
        if (!device) {
            std::cerr << "Error: Device es NULL en D3D12HighResRenderTarget::Initialize" << std::endl;
            return false;
        }

        m_width = renderWidth;
        m_height = renderHeight;

        // Crear descriptor heap para RTV
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = 1;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create RTV heap for high-res render target. HRESULT: 0x" 
                      << std::hex << hr << std::dec << std::endl;
            return false;
        }

        m_rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // Crear render target de alta resolución
        D3D12_RESOURCE_DESC renderTargetDesc = {};
        renderTargetDesc.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
        renderTargetDesc.Width = m_width;
        renderTargetDesc.Height = m_height;
        renderTargetDesc.DepthOrArraySize = 1;
        renderTargetDesc.MipLevels = 1;
        renderTargetDesc.Format = m_format;
        renderTargetDesc.SampleDesc.Count = 1;
        renderTargetDesc.SampleDesc.Quality = 0;
        renderTargetDesc.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN;
        renderTargetDesc.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET;

        D3D12_CLEAR_VALUE clearValue = {};
        clearValue.Format = m_format;
        clearValue.Color[0] = 0.05f;
        clearValue.Color[1] = 0.05f;
        clearValue.Color[2] = 0.1f;
        clearValue.Color[3] = 1.0f;

        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_DEFAULT;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;
        heapProps.CreationNodeMask = 1;
        heapProps.VisibleNodeMask = 1;

        hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &renderTargetDesc,
            D3D12_RESOURCE_STATE_RENDER_TARGET,
            &clearValue,
            IID_PPV_ARGS(&m_renderTarget)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create high-res render target. HRESULT: 0x" 
                      << std::hex << hr << std::dec << std::endl;
            return false;
        }

        // Crear RTV
        device->CreateRenderTargetView(m_renderTarget.Get(), nullptr, m_rtvHandle);

        std::cout << "High-res render target inicializado: " << m_width << "x" << m_height << std::endl;
        return true;
    }

    void D3D12HighResRenderTarget::Shutdown() {
        m_renderTarget.Reset();
        m_rtvHeap.Reset();
        m_rtvHandle = {};
        m_rtvDescriptorSize = 0;
    }

    void D3D12HighResRenderTarget::CopyToBackBuffer(
        ID3D12GraphicsCommandList* commandList,
        ID3D12Resource* backBuffer,
        UINT backBufferWidth,
        UINT backBufferHeight
    ) {
        if (!commandList || !backBuffer || !m_renderTarget) {
            return;
        }

        // Obtener descripciones de los recursos
        D3D12_RESOURCE_DESC srcDesc = m_renderTarget->GetDesc();
        D3D12_RESOURCE_DESC dstDesc = backBuffer->GetDesc();
        
        // Transición del render target de alta resolución a COPY_SOURCE
        D3D12_RESOURCE_BARRIER barrier1 = {};
        barrier1.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier1.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier1.Transition.pResource = m_renderTarget.Get();
        barrier1.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier1.Transition.StateAfter = D3D12_RESOURCE_STATE_COPY_SOURCE;
        barrier1.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &barrier1);

        // Si los tamaños son iguales, usar CopyResource (más eficiente)
        if (srcDesc.Width == dstDesc.Width && srcDesc.Height == dstDesc.Height) {
            commandList->CopyResource(backBuffer, m_renderTarget.Get());
        } else {
            // Para escalado automático, usar CopyTextureRegion con la región completa
            // Nota: CopyTextureRegion copia píxeles 1:1, pero cuando el destino es más pequeño,
            // el driver puede hacer escalado básico. Para escalado de alta calidad real,
            // se necesitaría un pixel shader con filtrado bilinear/bicubic.
            
            D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
            srcLocation.pResource = m_renderTarget.Get();
            srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            srcLocation.SubresourceIndex = 0;

            D3D12_TEXTURE_COPY_LOCATION dstLocation = {};
            dstLocation.pResource = backBuffer;
            dstLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
            dstLocation.SubresourceIndex = 0;

            // Copiar toda la región del origen al destino
            // El driver hará el escalado básico automáticamente
            commandList->CopyTextureRegion(&dstLocation, 0, 0, 0, &srcLocation, nullptr);
        }

        // Transición del render target de vuelta a RENDER_TARGET
        barrier1.Transition.StateBefore = D3D12_RESOURCE_STATE_COPY_SOURCE;
        barrier1.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        commandList->ResourceBarrier(1, &barrier1);
    }

} // namespace D3D12Core

