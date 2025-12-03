#include "D3D12SwapChain.h"
#include <d3d12.h>
#include <iostream>

namespace D3D12Core {

    D3D12SwapChain::D3D12SwapChain() {
    }

    D3D12SwapChain::~D3D12SwapChain() {
        Shutdown();
    }

    bool D3D12SwapChain::Initialize(
        HWND hwnd,
        IDXGIFactory4* factory,
        ID3D12CommandQueue* commandQueue,
        UINT width,
        UINT height,
        UINT bufferCount
    ) {
        m_factory = factory;
        m_commandQueue = commandQueue;
        m_bufferCount = bufferCount;
        m_width = width;
        m_height = height;

        if (!CreateSwapChain(hwnd, width, height)) {
            return false;
        }

        m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

        // Crear render target views
        ID3D12Device* device = nullptr;
        m_commandQueue->GetDevice(IID_PPV_ARGS(&device));
        if (device) {
            if (!CreateRenderTargetViews(device)) {
                device->Release();
                return false;
            }
            device->Release();
        }

        return true;
    }

    void D3D12SwapChain::Shutdown() {
        for (UINT i = 0; i < m_bufferCount; ++i) {
            m_backBuffers[i].Reset();
        }
        m_rtvHeap.Reset();
        m_swapChain.Reset();
    }

    void D3D12SwapChain::Resize(UINT width, UINT height) {
        if (m_width == width && m_height == height) {
            return;
        }

        m_width = width;
        m_height = height;

        // Liberar back buffers existentes
        for (UINT i = 0; i < m_bufferCount; ++i) {
            m_backBuffers[i].Reset();
        }

        // Resize swap chain
        HRESULT hr = m_swapChain->ResizeBuffers(
            m_bufferCount,
            width,
            height,
            m_format,
            DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to resize swap chain" << std::endl;
            return;
        }

        m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

        // Recrear render target views
        ID3D12Device* device = nullptr;
        m_commandQueue->GetDevice(IID_PPV_ARGS(&device));
        if (device) {
            CreateRenderTargetViews(device);
            device->Release();
        }
    }

    void D3D12SwapChain::Present(UINT syncInterval, UINT flags) {
        if (!m_swapChain) {
            return; // Swap chain no válido
        }
        
        HRESULT hr = m_swapChain->Present(syncInterval, flags);
        if (FAILED(hr)) {
            // Errores comunes que no deberían detener el loop:
            // DXGI_ERROR_DEVICE_REMOVED: GPU desconectada (raro)
            // DXGI_ERROR_DEVICE_RESET: GPU reseteada (raro)
            // Otros errores pueden ser temporales
            std::cerr << "Warning: Present falló. HRESULT: 0x" << std::hex << hr << std::dec 
                      << ". Continuando..." << std::endl;
            return; // No actualizar índice si falla
        }

        m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
    }

    ID3D12Resource* D3D12SwapChain::GetCurrentBackBuffer() const {
        return m_backBuffers[m_currentBackBufferIndex].Get();
    }

    D3D12_CPU_DESCRIPTOR_HANDLE D3D12SwapChain::GetCurrentRTV() const {
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        rtvHandle.ptr += m_currentBackBufferIndex * m_rtvDescriptorSize;
        return rtvHandle;
    }

    bool D3D12SwapChain::CreateSwapChain(HWND hwnd, UINT width, UINT height) {
        DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};
        swapChainDesc.Width = width;
        swapChainDesc.Height = height;
        swapChainDesc.Format = m_format;
        swapChainDesc.Stereo = FALSE;
        swapChainDesc.SampleDesc.Count = 1;
        swapChainDesc.SampleDesc.Quality = 0;
        swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.BufferCount = m_bufferCount;
        swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
        swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
        swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED;
        swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

        ComPtr<IDXGISwapChain1> swapChain1;
        HRESULT hr = m_factory->CreateSwapChainForHwnd(
            m_commandQueue,
            hwnd,
            &swapChainDesc,
            nullptr,
            nullptr,
            &swapChain1
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create swap chain" << std::endl;
            return false;
        }

        hr = swapChain1.As(&m_swapChain);
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to query swap chain interface" << std::endl;
            return false;
        }

        // Prevenir que DXGI cambie el modo de pantalla
        m_factory->MakeWindowAssociation(hwnd, DXGI_MWA_NO_ALT_ENTER);

        return true;
    }

    bool D3D12SwapChain::CreateRenderTargetViews(ID3D12Device* device) {
        // Crear descriptor heap para RTVs
        D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
        rtvHeapDesc.NumDescriptors = m_bufferCount;
        rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
        rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

        HRESULT hr = device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_rtvHeap));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create RTV descriptor heap" << std::endl;
            return false;
        }

        m_rtvDescriptorSize = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

        // Crear RTV para cada back buffer
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_rtvHeap->GetCPUDescriptorHandleForHeapStart();
        for (UINT i = 0; i < m_bufferCount; ++i) {
            hr = m_swapChain->GetBuffer(i, IID_PPV_ARGS(&m_backBuffers[i]));
            if (FAILED(hr)) {
                std::cerr << "Error: Failed to get back buffer " << i << std::endl;
                return false;
            }

            device->CreateRenderTargetView(m_backBuffers[i].Get(), nullptr, rtvHandle);
            rtvHandle.ptr += m_rtvDescriptorSize;
        }

        return true;
    }

} // namespace D3D12Core

