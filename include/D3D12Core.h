#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <memory>
#include <vector>
#include <string>

using Microsoft::WRL::ComPtr;

namespace D3D12Core {

    // Constantes para RTX 3060 12GB
    constexpr UINT BACK_BUFFER_COUNT = 3;
    constexpr DXGI_FORMAT BACK_BUFFER_FORMAT = DXGI_FORMAT_R8G8B8A8_UNORM;
    constexpr UINT MAX_FRAMES_IN_FLIGHT = 3;

    class D3D12Device;
    class D3D12CommandQueue;
    class D3D12SwapChain;
    class D3D12DescriptorHeap;

    // Clase principal que gestiona DirectX 12
    class D3D12Core {
    public:
        D3D12Core();
        ~D3D12Core();

        bool Initialize(HWND hwnd, UINT width, UINT height);
        void Shutdown();

        void BeginFrame();
        void EndFrame();
        void Present();

        // Getters
        D3D12Device* GetDevice() const { return m_device.get(); }
        D3D12CommandQueue* GetCommandQueue() const { return m_commandQueue.get(); }
        D3D12SwapChain* GetSwapChain() const { return m_swapChain.get(); }
        UINT GetCurrentBackBufferIndex() const { return m_currentBackBufferIndex; }
        UINT GetFrameIndex() const { return m_frameIndex; }

        // Resize
        void Resize(UINT width, UINT height);

    private:
        std::unique_ptr<D3D12Device> m_device;
        std::unique_ptr<D3D12CommandQueue> m_commandQueue;
        std::unique_ptr<D3D12SwapChain> m_swapChain;

        UINT m_currentBackBufferIndex = 0;
        UINT m_frameIndex = 0;
        UINT m_width = 0;
        UINT m_height = 0;
        HWND m_hwnd = nullptr;
    };

} // namespace D3D12Core

