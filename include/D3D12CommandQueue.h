#pragma once

#include "D3D12Core.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <queue>

namespace D3D12Core {

    class D3D12CommandQueue {
    public:
        D3D12CommandQueue();
        ~D3D12CommandQueue();

        bool Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type = D3D12_COMMAND_LIST_TYPE_DIRECT);
        void Shutdown();

        ID3D12CommandQueue* GetQueue() const { return m_commandQueue.Get(); }
        ID3D12CommandAllocator* GetCurrentAllocator() const;
        ID3D12GraphicsCommandList* GetCommandList() const { return m_commandList.Get(); }

        // Sincronización
        void ExecuteCommandList();
        void ResetCommandList();
        void WaitForGPU();
        void Flush();

        // Fence para sincronización
        UINT64 Signal();
        void WaitForFenceValue(UINT64 fenceValue);

    private:
        ComPtr<ID3D12CommandQueue> m_commandQueue;
        ComPtr<ID3D12CommandAllocator> m_commandAllocators[MAX_FRAMES_IN_FLIGHT];
        ComPtr<ID3D12GraphicsCommandList> m_commandList;
        ComPtr<ID3D12Fence> m_fence;

        UINT64 m_fenceValue = 0;
        HANDLE m_fenceEvent = nullptr;
        UINT m_frameIndex = 0;
        D3D12_COMMAND_LIST_TYPE m_type;
    };

} // namespace D3D12Core

