#pragma once

#include "D3D12Core.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace D3D12Core {

    class D3D12SwapChain {
    public:
        D3D12SwapChain();
        ~D3D12SwapChain();

        bool Initialize(
            HWND hwnd,
            IDXGIFactory4* factory,
            ID3D12CommandQueue* commandQueue,
            UINT width,
            UINT height,
            UINT bufferCount = BACK_BUFFER_COUNT
        );
        void Shutdown();

        void Resize(UINT width, UINT height);
        void Present(UINT syncInterval = 1, UINT flags = 0);

        ID3D12Resource* GetCurrentBackBuffer() const;
        UINT GetCurrentBackBufferIndex() const { return m_currentBackBufferIndex; }
        UINT GetBufferCount() const { return m_bufferCount; }
        DXGI_FORMAT GetFormat() const { return m_format; }

        // Render target views
        D3D12_CPU_DESCRIPTOR_HANDLE GetCurrentRTV() const;

    private:
        ComPtr<IDXGISwapChain3> m_swapChain;
        ComPtr<ID3D12Resource> m_backBuffers[BACK_BUFFER_COUNT];
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;

        UINT m_bufferCount = 0;
        UINT m_currentBackBufferIndex = 0;
        UINT m_rtvDescriptorSize = 0;
        DXGI_FORMAT m_format = BACK_BUFFER_FORMAT;
        UINT m_width = 0;
        UINT m_height = 0;

        ID3D12CommandQueue* m_commandQueue = nullptr;
        IDXGIFactory4* m_factory = nullptr;

        bool CreateSwapChain(HWND hwnd, UINT width, UINT height);
        bool CreateRenderTargetViews(ID3D12Device* device);
    };

} // namespace D3D12Core

