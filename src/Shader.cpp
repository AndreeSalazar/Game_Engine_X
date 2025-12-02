#include "../include/Shader.h"
#include <fstream>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <d3dcompiler.h>
#include <wrl/client.h>

#pragma comment(lib, "d3dcompiler.lib")

using Microsoft::WRL::ComPtr;

namespace D3D12Core {

    Shader::Shader() {
    }

    Shader::~Shader() {
    }

    bool Shader::LoadFromFile(const std::wstring& filename) {
        std::ifstream file(filename, std::ios::binary | std::ios::ate);
        if (!file.is_open()) {
            std::wcerr << L"Error: Failed to open shader file: " << filename << std::endl;
            return false;
        }

        std::streamsize size = file.tellg();
        file.seekg(0, std::ios::beg);

        m_bytecode.resize(size);
        if (!file.read(reinterpret_cast<char*>(m_bytecode.data()), size)) {
            std::cerr << "Error: Failed to read shader file" << std::endl;
            return false;
        }

        m_bytecodeSize = size;
        return true;
    }

    bool Shader::LoadFromBytecode(const void* bytecode, SIZE_T bytecodeSize) {
        m_bytecode.resize(bytecodeSize);
        memcpy(m_bytecode.data(), bytecode, bytecodeSize);
        m_bytecodeSize = bytecodeSize;
        return true;
    }

    D3D12_SHADER_BYTECODE Shader::GetBytecode() const {
        D3D12_SHADER_BYTECODE bytecode = {};
        if (m_bytecodeSize > 0) {
            bytecode.pShaderBytecode = m_bytecode.data();
            bytecode.BytecodeLength = m_bytecodeSize;
        }
        return bytecode;
    }

    bool ShaderCompiler::CompileShader(
        const std::wstring& filename,
        const std::string& entryPoint,
        const std::string& target,
        std::vector<BYTE>& outBytecode,
        std::string& outError
    ) {
        UINT compileFlags = 0;
#ifdef _DEBUG
        compileFlags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

        ComPtr<ID3DBlob> blob;
        ComPtr<ID3DBlob> errorBlob;

        std::wstring wideFilename = filename;
        HRESULT hr = D3DCompileFromFile(
            wideFilename.c_str(),
            nullptr,
            D3D_COMPILE_STANDARD_FILE_INCLUDE,
            entryPoint.c_str(),
            target.c_str(),
            compileFlags,
            0,
            &blob,
            &errorBlob
        );

        if (FAILED(hr)) {
            if (errorBlob) {
                outError = static_cast<const char*>(errorBlob->GetBufferPointer());
            }
            return false;
        }

        SIZE_T bufferSize = blob->GetBufferSize();
        outBytecode.resize(bufferSize);
        if (bufferSize > 0) {
            memcpy(outBytecode.data(), blob->GetBufferPointer(), bufferSize);
        }

        return true;
    }

} // namespace D3D12Core

