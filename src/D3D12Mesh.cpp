#include "../include/D3D12Mesh.h"
#include "../include/D3D12Buffer.h"
#include "../include/D3D12PipelineState.h"
#include <d3d12.h>
#include <wrl/client.h>
#include <iostream>

using Microsoft::WRL::ComPtr;

namespace D3D12Core {

    D3D12Mesh::D3D12Mesh() {
    }

    D3D12Mesh::~D3D12Mesh() {
        Shutdown();
    }

    bool D3D12Mesh::Initialize(
        ID3D12Device* device,
        ID3D12CommandQueue* commandQueue,
        const std::vector<Vertex>& vertices,
        const std::vector<UINT>& indices
    ) {
        // Crear vertex buffer (los buffers siempre se crean en COMMON, luego se transicionan)
        UINT64 vertexBufferSize = vertices.size() * sizeof(Vertex);
        m_vertexBuffer = std::make_unique<D3D12Buffer>();
        if (!m_vertexBuffer->Initialize(device, vertexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER)) {
            return false;
        }

        // Crear index buffer (los buffers siempre se crean en COMMON, luego se transicionan)
        UINT64 indexBufferSize = indices.size() * sizeof(UINT);
        m_indexBuffer = std::make_unique<D3D12Buffer>();
        if (!m_indexBuffer->Initialize(device, indexBufferSize, D3D12_RESOURCE_FLAG_NONE, D3D12_RESOURCE_STATE_INDEX_BUFFER)) {
            return false;
        }

        m_indexCount = static_cast<UINT>(indices.size());

        // Crear command list temporal para subir datos
        ComPtr<ID3D12CommandAllocator> tempAllocator;
        ComPtr<ID3D12GraphicsCommandList> tempCommandList;

        HRESULT hr = device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&tempAllocator));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create temp command allocator" << std::endl;
            return false;
        }

        hr = device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, tempAllocator.Get(), nullptr, IID_PPV_ARGS(&tempCommandList));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create temp command list" << std::endl;
            return false;
        }

        // Subir datos
        if (!m_vertexBuffer->UploadData(device, tempCommandList.Get(), vertices.data(), vertexBufferSize)) {
            return false;
        }

        if (!m_indexBuffer->UploadData(device, tempCommandList.Get(), indices.data(), indexBufferSize)) {
            return false;
        }

        // Cerrar y ejecutar
        tempCommandList->Close();
        ID3D12CommandList* commandLists[] = { tempCommandList.Get() };
        
        // Obtener la interfaz ID3D12CommandQueue
        ID3D12CommandQueue* d3dCommandQueue = commandQueue;
        d3dCommandQueue->ExecuteCommandLists(1, commandLists);

        // Esperar a que termine la copia
        ComPtr<ID3D12Fence> fence;
        UINT64 fenceValue = 1;
        hr = device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
        if (SUCCEEDED(hr)) {
            ID3D12CommandQueue* d3dCommandQueue = commandQueue;
            hr = d3dCommandQueue->Signal(fence.Get(), fenceValue);
            if (SUCCEEDED(hr)) {
                HANDLE fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
                if (fenceEvent) {
                    if (fence->GetCompletedValue() < fenceValue) {
                        hr = fence->SetEventOnCompletion(fenceValue, fenceEvent);
                        if (SUCCEEDED(hr)) {
                            WaitForSingleObject(fenceEvent, INFINITE);
                        }
                    }
                    CloseHandle(fenceEvent);
                }
            }
        }

        // Crear vertex buffer view
        m_vertexBufferView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
        m_vertexBufferView.SizeInBytes = static_cast<UINT>(vertexBufferSize);
        m_vertexBufferView.StrideInBytes = sizeof(Vertex);

        // Crear index buffer view
        m_indexBufferView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
        m_indexBufferView.SizeInBytes = static_cast<UINT>(indexBufferSize);
        m_indexBufferView.Format = DXGI_FORMAT_R32_UINT;

        return true;
    }

    void D3D12Mesh::Shutdown() {
        m_indexBuffer.reset();
        m_vertexBuffer.reset();
    }

    void D3D12Mesh::Draw(ID3D12GraphicsCommandList* commandList) {
        if (!commandList) {
            std::cerr << "Error: Command list is null in Draw()" << std::endl;
            return;
        }
        
        if (m_indexCount == 0) {
            std::cerr << "Error: Index count is 0 in Draw()" << std::endl;
            return;
        }
        
        commandList->IASetVertexBuffers(0, 1, &m_vertexBufferView);
        commandList->IASetIndexBuffer(&m_indexBufferView);
        commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
        commandList->DrawIndexedInstanced(m_indexCount, 1, 0, 0, 0);
    }

} // namespace D3D12Core

