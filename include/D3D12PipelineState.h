#pragma once

#include "D3D12Core.h"
#include "Shader.h"
#include <d3d12.h>
#include <wrl/client.h>

namespace D3D12Core {

    struct Vertex {
        float position[3];
        float color[3];
    };

    class D3D12PipelineState {
    public:
        D3D12PipelineState();
        ~D3D12PipelineState();

        bool Initialize(
            ID3D12Device* device,
            const Shader& vertexShader,
            const Shader& pixelShader,
            DXGI_FORMAT rtvFormat
        );
        void Shutdown();

        ID3D12PipelineState* GetPSO() const { return m_pipelineState.Get(); }
        ID3D12RootSignature* GetRootSignature() const { return m_rootSignature.Get(); }
        bool HasConstantBuffer() const { return m_hasConstantBuffer; }

    private:
        ComPtr<ID3D12RootSignature> m_rootSignature;
        ComPtr<ID3D12PipelineState> m_pipelineState;
        bool m_hasConstantBuffer = false;

        bool CreateRootSignature(ID3D12Device* device, bool useConstantBuffer = true);
        bool CreatePipelineState(
            ID3D12Device* device,
            const Shader& vertexShader,
            const Shader& pixelShader,
            DXGI_FORMAT rtvFormat
        );
    };

} // namespace D3D12Core

