#include "D3D12CommandQueue.h"
#include "D3D12Core.h"
#include <iostream>
#include <unknwn.h>

namespace D3D12Core {

    D3D12CommandQueue::D3D12CommandQueue() {
    }

    D3D12CommandQueue::~D3D12CommandQueue() {
        Shutdown();
    }

    bool D3D12CommandQueue::Initialize(ID3D12Device* device, D3D12_COMMAND_LIST_TYPE type) {
        m_type = type;

        // Crear cola de comandos
        D3D12_COMMAND_QUEUE_DESC queueDesc = {};
        queueDesc.Type = type;
        queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
        queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
        queueDesc.NodeMask = 0;

        HRESULT hr = device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_commandQueue));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create command queue" << std::endl;
            return false;
        }

        // Crear allocators para cada frame
        for (UINT i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            hr = device->CreateCommandAllocator(type, IID_PPV_ARGS(&m_commandAllocators[i]));
            if (FAILED(hr)) {
                std::cerr << "Error: Failed to create command allocator" << std::endl;
                return false;
            }
        }

        // Crear command list
        hr = device->CreateCommandList(
            0,
            type,
            m_commandAllocators[0].Get(),
            nullptr,
            IID_PPV_ARGS(&m_commandList)
        );
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create command list" << std::endl;
            return false;
        }

        m_commandList->Close();

        // Crear fence
        hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_fence));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create fence" << std::endl;
            return false;
        }

        m_fenceValue = 0;
        m_fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
        if (m_fenceEvent == nullptr) {
            std::cerr << "Error: Failed to create fence event" << std::endl;
            return false;
        }

        return true;
    }

    void D3D12CommandQueue::Shutdown() {
        WaitForGPU();

        if (m_fenceEvent) {
            CloseHandle(m_fenceEvent);
            m_fenceEvent = nullptr;
        }

        m_commandList.Reset();
        for (UINT i = 0; i < MAX_FRAMES_IN_FLIGHT; ++i) {
            m_commandAllocators[i].Reset();
        }
        m_commandQueue.Reset();
        m_fence.Reset();
    }

    ID3D12CommandAllocator* D3D12CommandQueue::GetCurrentAllocator() const {
        return m_commandAllocators[m_frameIndex].Get();
    }

    void D3D12CommandQueue::ExecuteCommandList() {
        HRESULT hr = m_commandList->Close();
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to close command list" << std::endl;
            return;
        }

        ID3D12CommandList* commandLists[] = { m_commandList.Get() };
        m_commandQueue->ExecuteCommandLists(1, commandLists);
    }

    void D3D12CommandQueue::ResetCommandList() {
        ID3D12CommandAllocator* allocator = GetCurrentAllocator();
        allocator->Reset();
        m_commandList->Reset(allocator, nullptr);
    }

    void D3D12CommandQueue::WaitForGPU() {
        UINT64 fenceValueToWaitFor = Signal();
        WaitForFenceValue(fenceValueToWaitFor);
    }

    void D3D12CommandQueue::Flush() {
        m_frameIndex = (m_frameIndex + 1) % MAX_FRAMES_IN_FLIGHT;
    }

    UINT64 D3D12CommandQueue::Signal() {
        m_fenceValue++;
        HRESULT hr = m_commandQueue->Signal(m_fence.Get(), m_fenceValue);
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to signal fence" << std::endl;
        }
        return m_fenceValue;
    }

    void D3D12CommandQueue::WaitForFenceValue(UINT64 fenceValue) {
        if (m_fence->GetCompletedValue() < fenceValue) {
            HRESULT hr = m_fence->SetEventOnCompletion(fenceValue, m_fenceEvent);
            if (FAILED(hr)) {
                std::cerr << "Error: Failed to set event on completion" << std::endl;
                return;
            }
            WaitForSingleObject(m_fenceEvent, INFINITE);
        }
    }

} // namespace D3D12Core

