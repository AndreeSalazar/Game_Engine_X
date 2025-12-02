#pragma once

#include <d3d12.h>
#include <string>
#include <vector>
#include <wrl/client.h>

namespace D3D12Core {

    class Shader {
    public:
        Shader();
        ~Shader();

        // Cargar shader desde archivo HLSL compilado
        bool LoadFromFile(const std::wstring& filename);
        
        // Cargar shader desde bytecode
        bool LoadFromBytecode(const void* bytecode, SIZE_T bytecodeSize);

        D3D12_SHADER_BYTECODE GetBytecode() const;
        bool IsValid() const { return m_bytecodeSize > 0; }

    private:
        std::vector<BYTE> m_bytecode;
        SIZE_T m_bytecodeSize = 0;
    };

    // Helper para compilar shaders HLSL
    class ShaderCompiler {
    public:
        static bool CompileShader(
            const std::wstring& filename,
            const std::string& entryPoint,
            const std::string& target,
            std::vector<BYTE>& outBytecode,
            std::string& outError
        );
    };

} // namespace D3D12Core

