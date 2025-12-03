#pragma once

#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>
#include <memory>

using Microsoft::WRL::ComPtr;

namespace D3D12Core {

    /// <summary>
    /// Render Target de alta resolución para mantener calidad máxima (1920x1080)
    /// mientras se escala automáticamente al tamaño del viewport
    /// </summary>
    class D3D12HighResRenderTarget {
    public:
        D3D12HighResRenderTarget();
        ~D3D12HighResRenderTarget();

        // Inicializar con resolución fija de alta calidad
        bool Initialize(ID3D12Device* device, UINT renderWidth = 1920, UINT renderHeight = 1080);
        void Shutdown();

        // Obtener el render target view
        D3D12_CPU_DESCRIPTOR_HANDLE GetRTV() const { return m_rtvHandle; }
        
        // Obtener el recurso del render target
        ID3D12Resource* GetResource() const { return m_renderTarget.Get(); }
        
        // Obtener dimensiones de renderizado
        UINT GetWidth() const { return m_width; }
        UINT GetHeight() const { return m_height; }

        // Copiar el render target de alta resolución al back buffer con escalado
        void CopyToBackBuffer(
            ID3D12GraphicsCommandList* commandList,
            ID3D12Resource* backBuffer,
            UINT backBufferWidth,
            UINT backBufferHeight
        );

    private:
        ComPtr<ID3D12Resource> m_renderTarget;
        ComPtr<ID3D12DescriptorHeap> m_rtvHeap;
        D3D12_CPU_DESCRIPTOR_HANDLE m_rtvHandle = {};
        UINT m_rtvDescriptorSize = 0;
        UINT m_width = 1920;
        UINT m_height = 1080;
        DXGI_FORMAT m_format = DXGI_FORMAT_R8G8B8A8_UNORM;
    };

} // namespace D3D12Core

