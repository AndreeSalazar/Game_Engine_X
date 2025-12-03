#pragma once

#include "D3D12Core.h"
#include "D3D12Buffer.h"
#include "D3D12PipelineState.h"
#include <d3d12.h>
#include <vector>

namespace D3D12Core {

    class D3D12Mesh {
    public:
        D3D12Mesh();
        ~D3D12Mesh();

        bool Initialize(
            ID3D12Device* device,
            ID3D12CommandQueue* commandQueue,
            const std::vector<Vertex>& vertices,
            const std::vector<UINT>& indices
        );
        void Shutdown();

        void Draw(ID3D12GraphicsCommandList* commandList);

        UINT GetIndexCount() const { return m_indexCount; }

    private:
        std::unique_ptr<D3D12Buffer> m_vertexBuffer;
        std::unique_ptr<D3D12Buffer> m_indexBuffer;
        D3D12_VERTEX_BUFFER_VIEW m_vertexBufferView = {};
        D3D12_INDEX_BUFFER_VIEW m_indexBufferView = {};
        UINT m_indexCount = 0;
    };

} // namespace D3D12Core

