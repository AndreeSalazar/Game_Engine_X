#include "D3D12Core.h"
#include "D3D12Device.h"
#include "D3D12CommandQueue.h"
#include "D3D12SwapChain.h"
#include <iostream>
#include <iomanip>

namespace D3D12Core {

    D3D12Core::D3D12Core() {
    }

    D3D12Core::~D3D12Core() {
        Shutdown();
    }

    bool D3D12Core::Initialize(HWND hwnd, UINT width, UINT height) {
        m_hwnd = hwnd;
        m_width = width;
        m_height = height;

        // Crear device
        m_device = std::make_unique<D3D12Device>();
        if (!m_device->Initialize()) {
            std::cerr << "Error: Failed to initialize D3D12 Device" << std::endl;
            return false;
        }

        // Crear command queue
        m_commandQueue = std::make_unique<D3D12CommandQueue>();
        if (!m_commandQueue->Initialize(m_device->GetDevice())) {
            std::cerr << "Error: Failed to initialize Command Queue" << std::endl;
            return false;
        }

        // Crear swap chain
        m_swapChain = std::make_unique<D3D12SwapChain>();
        if (!m_swapChain->Initialize(
            hwnd,
            m_device->GetFactory(),
            m_commandQueue->GetQueue(),
            width,
            height
        )) {
            std::cerr << "Error: Failed to initialize Swap Chain" << std::endl;
            return false;
        }

        // Crear render target views
        ID3D12Device* device = m_device->GetDevice();
        // Esto se hace internamente en CreateRenderTargetViews

        std::wcout << L"DirectX 12 inicializado correctamente" << std::endl;
        auto adapterInfo = m_device->GetAdapterInfo();
        std::wcout << L"Adaptador: " << adapterInfo.Description.c_str() << std::endl;
        std::wcout << L"Memoria de video dedicada: " << (adapterInfo.DedicatedVideoMemory / (1024ULL * 1024ULL * 1024ULL)) << L" GB" << std::endl;

        return true;
    }

    void D3D12Core::Shutdown() {
        if (m_commandQueue) {
            m_commandQueue->WaitForGPU();
        }

        m_swapChain.reset();
        m_commandQueue.reset();
        m_device.reset();
    }

    void D3D12Core::BeginFrame() {
        // Reset command list
        m_commandQueue->ResetCommandList();
        ID3D12GraphicsCommandList* commandList = m_commandQueue->GetCommandList();

        // Obtener back buffer actual
        m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();
        ID3D12Resource* backBuffer = m_swapChain->GetCurrentBackBuffer();

        // Transición a render target
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = backBuffer;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        commandList->ResourceBarrier(1, &barrier);

        // Limpiar render target con color oscuro elegante
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain->GetCurrentRTV();
        const float clearColor[] = { 0.05f, 0.05f, 0.1f, 1.0f }; // Azul muy oscuro elegante
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        // Establecer render target
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
    }

    void D3D12Core::EndFrame() {
        ID3D12GraphicsCommandList* commandList = m_commandQueue->GetCommandList();
        ID3D12Resource* backBuffer = m_swapChain->GetCurrentBackBuffer();

        // Transición a present
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = backBuffer;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;

        commandList->ResourceBarrier(1, &barrier);

        // NO cerrar aquí - ExecuteCommandList ya lo hace
        // commandList->Close();

        // Ejecutar comandos (esto cierra el command list internamente)
        m_commandQueue->ExecuteCommandList();
    }

    void D3D12Core::Present() {
        if (!m_swapChain) {
            return; // Swap chain no válido
        }
        
        // Present retorna void, el manejo de errores está en D3D12SwapChain::Present()
        m_swapChain->Present(1, 0); // VSync habilitado
        
        m_commandQueue->Flush();
        m_frameIndex++;
    }

    void D3D12Core::Resize(UINT width, UINT height) {
        if (m_width == width && m_height == height) {
            return;
        }

        m_commandQueue->WaitForGPU();

        m_width = width;
        m_height = height;

        m_swapChain->Resize(width, height);
    }

} // namespace D3D12Core

