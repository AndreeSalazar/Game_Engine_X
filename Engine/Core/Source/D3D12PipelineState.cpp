#include "D3D12PipelineState.h"
#include <d3d12.h>
#include <d3dcompiler.h>
#include <d3dcommon.h>
#include <iostream>
#include <wrl/client.h>

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

namespace D3D12Core {

    D3D12PipelineState::D3D12PipelineState() {
    }

    D3D12PipelineState::~D3D12PipelineState() {
        Shutdown();
    }

    bool D3D12PipelineState::Initialize(
        ID3D12Device* device,
        const Shader& vertexShader,
        const Shader& pixelShader,
        DXGI_FORMAT rtvFormat
    ) {
        if (!CreateRootSignature(device, true)) {
            return false;
        }

        if (!CreatePipelineState(device, vertexShader, pixelShader, rtvFormat)) {
            return false;
        }

        return true;
    }

    void D3D12PipelineState::Shutdown() {
        m_pipelineState.Reset();
        m_rootSignature.Reset();
    }

    bool D3D12PipelineState::CreateRootSignature(ID3D12Device* device, bool useConstantBuffer) {
        m_hasConstantBuffer = useConstantBuffer;
        
        D3D12_ROOT_PARAMETER rootParameters[1] = {};
        UINT numParameters = 0;

        if (useConstantBuffer) {
            // Constant buffer para matrices MVP
            rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
            rootParameters[0].Descriptor.ShaderRegister = 0;
            rootParameters[0].Descriptor.RegisterSpace = 0;
            rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;
            numParameters = 1;
        }

        D3D12_ROOT_SIGNATURE_DESC rootSignatureDesc = {};
        rootSignatureDesc.NumParameters = numParameters;
        rootSignatureDesc.pParameters = numParameters > 0 ? rootParameters : nullptr;
        rootSignatureDesc.NumStaticSamplers = 0;
        rootSignatureDesc.pStaticSamplers = nullptr;
        rootSignatureDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        HRESULT hr = D3D12SerializeRootSignature(
            &rootSignatureDesc,
            D3D_ROOT_SIGNATURE_VERSION_1,
            &signature,
            &error
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to serialize root signature" << std::endl;
            if (error) {
                std::cerr << static_cast<const char*>(error->GetBufferPointer()) << std::endl;
            }
            return false;
        }

        hr = device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&m_rootSignature)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create root signature" << std::endl;
            return false;
        }

        return true;
    }

    bool D3D12PipelineState::CreatePipelineState(
        ID3D12Device* device,
        const Shader& vertexShader,
        const Shader& pixelShader,
        DXGI_FORMAT rtvFormat
    ) {
        // Input layout
        D3D12_INPUT_ELEMENT_DESC inputElementDescs[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };

        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = vertexShader.GetBytecode();
        psoDesc.PS = pixelShader.GetBytecode();
        // Blend state - asegurar que los colores se escriban correctamente
        psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
        psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
        
        psoDesc.SampleMask = UINT_MAX;
        
        // Rasterizer state - desactivar culling temporalmente para debug
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE; // Cambiado de BACK a NONE para ver todas las caras
        psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
        psoDesc.RasterizerState.DepthBias = 0;
        psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
        psoDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
        psoDesc.RasterizerState.DepthClipEnable = TRUE;
        psoDesc.RasterizerState.MultisampleEnable = FALSE;
        psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
        psoDesc.RasterizerState.ForcedSampleCount = 0;
        psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        
        // Depth stencil - desactivado por ahora
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = rtvFormat;
        psoDesc.SampleDesc.Count = 1;

        HRESULT hr = device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pipelineState));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create pipeline state" << std::endl;
            return false;
        }

        return true;
    }

} // namespace D3D12Core

