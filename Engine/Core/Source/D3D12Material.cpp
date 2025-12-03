#include "D3D12Material.h"
#include "D3D12PipelineState.h"
#include "Shader.h"
#include <fstream>
#include <sstream>
#include <d3dcompiler.h>
#include <d3d12.h>
#include <iostream>

#pragma comment(lib, "d3dcompiler.lib")

namespace D3D12Core {

    D3D12Material::D3D12Material() {
    }

    D3D12Material::~D3D12Material() {
        if (m_constantBufferMapped && m_constantBuffer) {
            m_constantBuffer->Unmap(0, nullptr);
        }
    }

    bool D3D12Material::Initialize(
        ID3D12Device* device,
        const std::string& materialName,
        const std::string& vertexShaderPath,
        const std::string& pixelShaderPath)
    {
        m_device = device;
        m_materialName = materialName;

        // Crear root signature
        CreateRootSignature();
        
        if (!m_rootSignature) {
            std::cerr << "Error: Failed to create root signature for material" << std::endl;
            return false;
        }

        // Crear pipeline state
        CreatePipelineState(vertexShaderPath, pixelShaderPath);
        
        if (!m_pso) {
            std::cerr << "Error: Failed to create pipeline state for material" << std::endl;
            return false;
        }

        // Crear constant buffer para parámetros
        D3D12_HEAP_PROPERTIES heapProps = {};
        heapProps.Type = D3D12_HEAP_TYPE_UPLOAD;
        heapProps.CPUPageProperty = D3D12_CPU_PAGE_PROPERTY_UNKNOWN;
        heapProps.MemoryPoolPreference = D3D12_MEMORY_POOL_UNKNOWN;

        D3D12_RESOURCE_DESC bufferDesc = {};
        bufferDesc.Dimension = D3D12_RESOURCE_DIMENSION_BUFFER;
        bufferDesc.Width = m_constantBufferSize;
        bufferDesc.Height = 1;
        bufferDesc.DepthOrArraySize = 1;
        bufferDesc.MipLevels = 1;
        bufferDesc.Format = DXGI_FORMAT_UNKNOWN;
        bufferDesc.SampleDesc.Count = 1;
        bufferDesc.Layout = D3D12_TEXTURE_LAYOUT_ROW_MAJOR;
        bufferDesc.Flags = D3D12_RESOURCE_FLAG_NONE;

        HRESULT hr = device->CreateCommittedResource(
            &heapProps,
            D3D12_HEAP_FLAG_NONE,
            &bufferDesc,
            D3D12_RESOURCE_STATE_GENERIC_READ,
            nullptr,
            IID_PPV_ARGS(&m_constantBuffer)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create material constant buffer" << std::endl;
            return false;
        }

        // Mapear constant buffer
        D3D12_RANGE readRange = { 0, 0 };
        m_constantBuffer->Map(0, &readRange, &m_constantBufferMapped);

        return true;
    }

    void D3D12Material::SetScalar(const std::string& name, float value) {
        MaterialParameter param;
        param.type = MaterialParameterType::Scalar;
        param.name = name;
        param.value.scalar = value;
        param.isDirty = true;
        m_parameters[name] = param;
        UpdateConstantBuffer();
    }

    void D3D12Material::SetVector2(const std::string& name, const DirectX::XMFLOAT2& value) {
        MaterialParameter param;
        param.type = MaterialParameterType::Vector2;
        param.name = name;
        param.value.vector2 = value;
        param.isDirty = true;
        m_parameters[name] = param;
        UpdateConstantBuffer();
    }

    void D3D12Material::SetVector3(const std::string& name, const DirectX::XMFLOAT3& value) {
        MaterialParameter param;
        param.type = MaterialParameterType::Vector3;
        param.name = name;
        param.value.vector3 = value;
        param.isDirty = true;
        m_parameters[name] = param;
        UpdateConstantBuffer();
    }

    void D3D12Material::SetVector4(const std::string& name, const DirectX::XMFLOAT4& value) {
        MaterialParameter param;
        param.type = MaterialParameterType::Vector4;
        param.name = name;
        param.value.vector4 = value;
        param.isDirty = true;
        m_parameters[name] = param;
        UpdateConstantBuffer();
    }

    float D3D12Material::GetScalar(const std::string& name, float defaultValue) const {
        auto it = m_parameters.find(name);
        if (it != m_parameters.end() && it->second.type == MaterialParameterType::Scalar) {
            return it->second.value.scalar;
        }
        return defaultValue;
    }

    DirectX::XMFLOAT3 D3D12Material::GetVector3(const std::string& name, const DirectX::XMFLOAT3& defaultValue) const {
        auto it = m_parameters.find(name);
        if (it != m_parameters.end() && it->second.type == MaterialParameterType::Vector3) {
            return it->second.value.vector3;
        }
        return defaultValue;
    }

    void D3D12Material::Bind(ID3D12GraphicsCommandList* commandList) {
        if (!m_pso || !m_rootSignature) {
            return;
        }

        // Establecer pipeline state
        commandList->SetPipelineState(m_pso.Get());
        commandList->SetGraphicsRootSignature(m_rootSignature.Get());

        // Nota: El constant buffer MVP (b0) debe ser bindeado desde fuera
        // ya que el Material System ahora usa el mismo root signature que el básico
        // Los parámetros del material se pueden agregar después sin romper compatibilidad
    }

    void D3D12Material::UpdateConstantBuffer() {
        if (!m_constantBufferMapped) return;

        // Empaquetar todos los parámetros en el constant buffer
        float* data = static_cast<float*>(m_constantBufferMapped);
        int offset = 0;

        for (auto& [name, param] : m_parameters) {
            if (param.isDirty) {
                switch (param.type) {
                case MaterialParameterType::Scalar:
                    data[offset++] = param.value.scalar;
                    break;
                case MaterialParameterType::Vector2:
                    data[offset++] = param.value.vector2.x;
                    data[offset++] = param.value.vector2.y;
                    offset += 2; // Padding
                    break;
                case MaterialParameterType::Vector3:
                    data[offset++] = param.value.vector3.x;
                    data[offset++] = param.value.vector3.y;
                    data[offset++] = param.value.vector3.z;
                    offset++; // Padding
                    break;
                case MaterialParameterType::Vector4:
                    data[offset++] = param.value.vector4.x;
                    data[offset++] = param.value.vector4.y;
                    data[offset++] = param.value.vector4.z;
                    data[offset++] = param.value.vector4.w;
                    break;
                }
                param.isDirty = false;
            }
        }
    }

    void D3D12Material::CreateRootSignature() {
        // Crear root signature compatible con el PSO básico
        // Por ahora, usar la misma estructura que el básico para compatibilidad
        D3D12_ROOT_PARAMETER rootParams[1] = {};
        
        // Constant buffer para MVP (register b0) - mismo que el básico
        // Nota: Los parámetros del material se pueden agregar después sin romper compatibilidad
        rootParams[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;
        rootParams[0].Descriptor.ShaderRegister = 0; // b0 para MVP (igual que el básico)
        rootParams[0].Descriptor.RegisterSpace = 0;
        rootParams[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;

        D3D12_ROOT_SIGNATURE_DESC rootSigDesc = {};
        rootSigDesc.NumParameters = 1;
        rootSigDesc.pParameters = rootParams;
        rootSigDesc.NumStaticSamplers = 0;
        rootSigDesc.pStaticSamplers = nullptr;
        rootSigDesc.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

        ComPtr<ID3DBlob> signature;
        ComPtr<ID3DBlob> error;
        HRESULT hr = D3D12SerializeRootSignature(&rootSigDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to serialize material root signature" << std::endl;
            if (error) {
                std::cerr << static_cast<const char*>(error->GetBufferPointer()) << std::endl;
            }
            return;
        }

        hr = m_device->CreateRootSignature(
            0,
            signature->GetBufferPointer(),
            signature->GetBufferSize(),
            IID_PPV_ARGS(&m_rootSignature)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create material root signature" << std::endl;
        }
    }

    void D3D12Material::CreatePipelineState(const std::string& vsPath, const std::string& psPath) {
        // Compilar shaders
        std::vector<BYTE> vsBytecode, psBytecode;
        std::string error;

        // Intentar múltiples rutas para los shaders
        std::vector<std::wstring> vsPaths = {
            std::wstring(vsPath.begin(), vsPath.end()),
            L"Engine/Rendering/Shaders/BasicVS.hlsl",
            L"Shaders/BasicVS.hlsl"
        };
        
        std::vector<std::wstring> psPaths = {
            std::wstring(psPath.begin(), psPath.end()),
            L"Engine/Rendering/Shaders/BasicPS.hlsl",
            L"Shaders/BasicPS.hlsl"
        };

        bool vsCompiled = false;
        for (const auto& path : vsPaths) {
            if (ShaderCompiler::CompileShader(path, "main", "vs_5_0", vsBytecode, error)) {
                vsCompiled = true;
                break;
            }
        }
        
        if (!vsCompiled) {
            std::cerr << "Error compiling vertex shader for material. Tried paths:" << std::endl;
            for (const auto& path : vsPaths) {
                std::wcerr << L"  - " << path << std::endl;
            }
            std::cerr << "Last error: " << error << std::endl;
            return;
        }

        bool psCompiled = false;
        for (const auto& path : psPaths) {
            if (ShaderCompiler::CompileShader(path, "main", "ps_5_0", psBytecode, error)) {
                psCompiled = true;
                break;
            }
        }
        
        if (!psCompiled) {
            std::cerr << "Error compiling pixel shader for material. Tried paths:" << std::endl;
            for (const auto& path : psPaths) {
                std::wcerr << L"  - " << path << std::endl;
            }
            std::cerr << "Last error: " << error << std::endl;
            return;
        }

        // Crear PSO
        D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
        psoDesc.pRootSignature = m_rootSignature.Get();
        psoDesc.VS = { vsBytecode.data(), vsBytecode.size() };
        psoDesc.PS = { psBytecode.data(), psBytecode.size() };
        
        // Input layout (debe coincidir exactamente con el PSO básico)
        D3D12_INPUT_ELEMENT_DESC inputLayout[] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
            { "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
        };
        psoDesc.InputLayout = { inputLayout, _countof(inputLayout) };
        
        // Blend state - debe coincidir con el básico
        psoDesc.BlendState.RenderTarget[0].BlendEnable = FALSE;
        psoDesc.BlendState.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
        psoDesc.BlendState.RenderTarget[0].LogicOpEnable = FALSE;
        
        psoDesc.SampleMask = UINT_MAX;
        
        // Rasterizer state - debe coincidir con el básico
        psoDesc.RasterizerState.FillMode = D3D12_FILL_MODE_SOLID;
        psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
        psoDesc.RasterizerState.FrontCounterClockwise = FALSE;
        psoDesc.RasterizerState.DepthBias = 0;
        psoDesc.RasterizerState.DepthBiasClamp = 0.0f;
        psoDesc.RasterizerState.SlopeScaledDepthBias = 0.0f;
        psoDesc.RasterizerState.DepthClipEnable = TRUE;
        psoDesc.RasterizerState.MultisampleEnable = FALSE;
        psoDesc.RasterizerState.AntialiasedLineEnable = FALSE;
        psoDesc.RasterizerState.ForcedSampleCount = 0;
        psoDesc.RasterizerState.ConservativeRaster = D3D12_CONSERVATIVE_RASTERIZATION_MODE_OFF;
        
        // Depth stencil - debe coincidir con el básico
        psoDesc.DepthStencilState.DepthEnable = FALSE;
        psoDesc.DepthStencilState.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;
        psoDesc.DepthStencilState.DepthFunc = D3D12_COMPARISON_FUNC_ALWAYS;
        psoDesc.DepthStencilState.StencilEnable = FALSE;
        
        psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
        psoDesc.NumRenderTargets = 1;
        psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
        psoDesc.SampleDesc.Count = 1;
        psoDesc.SampleDesc.Quality = 0;

        HRESULT hr = m_device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&m_pso));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create material pipeline state. HRESULT: 0x" 
                      << std::hex << hr << std::dec << std::endl;
            m_pso = nullptr; // Asegurar que sea null si falla
        }
    }

    bool D3D12Material::LoadFromFile(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            return false;
        }

        std::string json((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());
        file.close();

        return DeserializeFromJSON(json);
    }

    bool D3D12Material::DeserializeFromJSON(const std::string& json) {
        // Parsing básico de JSON (sin librerías externas)
        std::istringstream iss(json);
        std::string line;
        
        while (std::getline(iss, line)) {
            // Buscar parámetros
            if (line.find("\"BaseColor\"") != std::string::npos) {
                // Extraer valores RGB
                size_t start = line.find('[');
                size_t end = line.find(']');
                if (start != std::string::npos && end != std::string::npos) {
                    std::string values = line.substr(start + 1, end - start - 1);
                    std::istringstream valueStream(values);
                    float r, g, b;
                    char comma;
                    if (valueStream >> r >> comma >> g >> comma >> b) {
                        SetVector3("BaseColor", DirectX::XMFLOAT3(r, g, b));
                    }
                }
            }
            else if (line.find("\"Metallic\"") != std::string::npos) {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string value = line.substr(colon + 1);
                    value.erase(0, value.find_first_not_of(" \t,"));
                    value.erase(value.find_last_not_of(" \t,") + 1);
                    SetScalar("Metallic", std::stof(value));
                }
            }
            else if (line.find("\"Roughness\"") != std::string::npos) {
                size_t colon = line.find(':');
                if (colon != std::string::npos) {
                    std::string value = line.substr(colon + 1);
                    value.erase(0, value.find_first_not_of(" \t,"));
                    value.erase(value.find_last_not_of(" \t,") + 1);
                    SetScalar("Roughness", std::stof(value));
                }
            }
            else if (line.find("\"Emissive\"") != std::string::npos) {
                size_t start = line.find('[');
                size_t end = line.find(']');
                if (start != std::string::npos && end != std::string::npos) {
                    std::string values = line.substr(start + 1, end - start - 1);
                    std::istringstream valueStream(values);
                    float r, g, b;
                    char comma;
                    if (valueStream >> r >> comma >> g >> comma >> b) {
                        SetVector3("Emissive", DirectX::XMFLOAT3(r, g, b));
                    }
                }
            }
        }

        return true;
    }

    std::string D3D12Material::SerializeToJSON() const {
        std::ostringstream json;
        json << "{\n";
        json << "  \"name\": \"" << m_materialName << "\",\n";
        json << "  \"parameters\": {\n";
        
        bool first = true;
        for (const auto& [name, param] : m_parameters) {
            if (!first) json << ",\n";
            first = false;
            
            json << "    \"" << name << "\": {";
            json << "\"type\": \"";
            
            switch (param.type) {
            case MaterialParameterType::Scalar:
                json << "scalar\", \"value\": " << param.value.scalar;
                break;
            case MaterialParameterType::Vector3:
                json << "vector3\", \"value\": [" 
                     << param.value.vector3.x << ", "
                     << param.value.vector3.y << ", "
                     << param.value.vector3.z << "]";
                break;
            case MaterialParameterType::Vector4:
                json << "vector4\", \"value\": ["
                     << param.value.vector4.x << ", "
                     << param.value.vector4.y << ", "
                     << param.value.vector4.z << ", "
                     << param.value.vector4.w << "]";
                break;
            }
            json << "}";
        }
        
        json << "\n  }\n";
        json << "}";
        return json.str();
    }

    bool D3D12Material::ReloadShaders() {
        // TODO: Implementar recarga de shaders
        return true;
    }

    // Material Instance Implementation
    D3D12MaterialInstance::D3D12MaterialInstance(D3D12Material* baseMaterial) 
        : m_baseMaterial(baseMaterial) {
    }

    void D3D12MaterialInstance::SetScalarOverride(const std::string& name, float value) {
        MaterialParameter param;
        param.type = MaterialParameterType::Scalar;
        param.name = name;
        param.value.scalar = value;
        param.isDirty = true;
        m_overrides[name] = param;
    }

    void D3D12MaterialInstance::SetVector3Override(const std::string& name, const DirectX::XMFLOAT3& value) {
        MaterialParameter param;
        param.type = MaterialParameterType::Vector3;
        param.name = name;
        param.value.vector3 = value;
        param.isDirty = true;
        m_overrides[name] = param;
    }

    void D3D12MaterialInstance::SetVector4Override(const std::string& name, const DirectX::XMFLOAT4& value) {
        MaterialParameter param;
        param.type = MaterialParameterType::Vector4;
        param.name = name;
        param.value.vector4 = value;
        param.isDirty = true;
        m_overrides[name] = param;
    }

    void D3D12MaterialInstance::Bind(ID3D12GraphicsCommandList* commandList) {
        if (m_baseMaterial) {
            m_baseMaterial->Bind(commandList);
            // TODO: Aplicar overrides
        }
    }

} // namespace D3D12Core

