#pragma once

#include <d3d12.h>
#include <string>
#include <unordered_map>
#include <vector>
#include <DirectXMath.h>
#include <wrl/client.h>
#include "Shader.h"

using Microsoft::WRL::ComPtr;

namespace D3D12Core {

    // Tipos de parámetros de material
    enum class MaterialParameterType {
        Scalar,      // float
        Vector2,     // float2
        Vector3,     // float3
        Vector4,     // float4
        Texture2D,   // ID3D12Resource* (futuro)
        TextureCube  // ID3D12Resource* (futuro)
    };

    // Estructura de un parámetro de material
    struct MaterialParameter {
        MaterialParameterType type;
        std::string name;
        
        union {
            float scalar;
            DirectX::XMFLOAT2 vector2;
            DirectX::XMFLOAT3 vector3;
            DirectX::XMFLOAT4 vector4;
            ID3D12Resource* texture;
        } value;
        
        bool isDirty = true; // Necesita actualizar constant buffer
    };

    // Estructura de un material
    class D3D12Material {
    public:
        D3D12Material();
        ~D3D12Material();

        // Inicialización
        bool Initialize(
            ID3D12Device* device,
            const std::string& materialName,
            const std::string& vertexShaderPath,
            const std::string& pixelShaderPath
        );

        // Gestión de parámetros
        void SetScalar(const std::string& name, float value);
        void SetVector2(const std::string& name, const DirectX::XMFLOAT2& value);
        void SetVector3(const std::string& name, const DirectX::XMFLOAT3& value);
        void SetVector4(const std::string& name, const DirectX::XMFLOAT4& value);
        void SetTexture(const std::string& name, ID3D12Resource* texture);

        float GetScalar(const std::string& name, float defaultValue = 0.0f) const;
        DirectX::XMFLOAT2 GetVector2(const std::string& name, const DirectX::XMFLOAT2& defaultValue = {0,0}) const;
        DirectX::XMFLOAT3 GetVector3(const std::string& name, const DirectX::XMFLOAT3& defaultValue = {0,0,0}) const;
        DirectX::XMFLOAT4 GetVector4(const std::string& name, const DirectX::XMFLOAT4& defaultValue = {0,0,0,1}) const;

        // Aplicar material al pipeline
        void Bind(ID3D12GraphicsCommandList* commandList);

        // Hot-reload (recargar shaders sin reiniciar)
        bool ReloadShaders();

        // Serialización (para guardar/cargar)
        std::string SerializeToJSON() const;
        bool DeserializeFromJSON(const std::string& json);

        // Cargar desde archivo JSON
        bool LoadFromFile(const std::string& filepath);

        // Getters
        ID3D12PipelineState* GetPSO() const { return m_pso.Get(); }
        ID3D12RootSignature* GetRootSignature() const { return m_rootSignature.Get(); }
        const std::string& GetName() const { return m_materialName; }
        bool IsValid() const { return m_pso != nullptr && m_rootSignature != nullptr; }

    private:
        std::string m_materialName;
        
        // Shaders
        ComPtr<ID3D12PipelineState> m_pso;
        ComPtr<ID3D12RootSignature> m_rootSignature;
        
        // Parámetros del material
        std::unordered_map<std::string, MaterialParameter> m_parameters;
        
        // Constant buffer para parámetros
        ComPtr<ID3D12Resource> m_constantBuffer;
        void* m_constantBufferMapped = nullptr;
        UINT m_constantBufferSize = 256; // Tamaño mínimo, ajustar según parámetros
        
        // Device reference
        ID3D12Device* m_device = nullptr;

        // Helpers
        void UpdateConstantBuffer();
        void CreateRootSignature();
        void CreatePipelineState(const std::string& vsPath, const std::string& psPath);
        void ParseJSONParameter(const std::string& name, const std::string& type, const std::string& valueStr);
    };

    // Material Instance (variación de un material base)
    class D3D12MaterialInstance {
    public:
        D3D12MaterialInstance(D3D12Material* baseMaterial);
        
        // Override de parámetros
        void SetScalarOverride(const std::string& name, float value);
        void SetVector3Override(const std::string& name, const DirectX::XMFLOAT3& value);
        void SetVector4Override(const std::string& name, const DirectX::XMFLOAT4& value);
        
        // Aplicar (usa base material + overrides)
        void Bind(ID3D12GraphicsCommandList* commandList);

        D3D12Material* GetBaseMaterial() const { return m_baseMaterial; }

    private:
        D3D12Material* m_baseMaterial;
        std::unordered_map<std::string, MaterialParameter> m_overrides;
    };

} // namespace D3D12Core

