#pragma once

#include "D3D12Core.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace D3D12Core {

    class D3D12Buffer {
    public:
        D3D12Buffer();
        ~D3D12Buffer();

        bool Initialize(
            ID3D12Device* device,
            UINT64 size,
            D3D12_RESOURCE_FLAGS flags = D3D12_RESOURCE_FLAG_NONE,
            D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON
        );
        void Shutdown();

        ID3D12Resource* GetResource() const { return m_resource.Get(); }
        UINT64 GetSize() const { return m_size; }
        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

        // Upload data
        bool UploadData(ID3D12Device* device, ID3D12GraphicsCommandList* commandList, const void* data, UINT64 size);

    protected:
        ComPtr<ID3D12Resource> m_resource;
        ComPtr<ID3D12Resource> m_uploadBuffer;
        UINT64 m_size = 0;
        D3D12_RESOURCE_STATES m_currentState;

    private:
        friend class D3D12ConstantBuffer;
    };

} // namespace D3D12Core

