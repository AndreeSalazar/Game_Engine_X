#pragma once

#include "D3D12Core.h"
#include "D3D12Buffer.h"
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>

using namespace DirectX;

namespace D3D12Core {

    // Estructura para matrices MVP (Model-View-Projection)
    // Usar XMFLOAT4X4 en lugar de XMMATRIX para poder copiar a memoria
    struct MVPConstantBuffer {
        XMFLOAT4X4 model;
        XMFLOAT4X4 view;
        XMFLOAT4X4 projection;
    };

    class D3D12ConstantBuffer {
    public:
        D3D12ConstantBuffer();
        ~D3D12ConstantBuffer();

        bool Initialize(ID3D12Device* device, UINT size);
        void Shutdown();

        void UpdateData(const void* data, UINT size);
        void Bind(ID3D12GraphicsCommandList* commandList, UINT rootParameterIndex);

        D3D12_GPU_VIRTUAL_ADDRESS GetGPUVirtualAddress() const;

    private:
        ComPtr<ID3D12Resource> m_resource;
        UINT m_size;
        void* m_mappedData;
    };

} // namespace D3D12Core

