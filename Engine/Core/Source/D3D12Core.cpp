#include "D3D12Core.h"
#include "D3D12Device.h"
#include "D3D12CommandQueue.h"
#include "D3D12SwapChain.h"
#include "D3D12HighResRenderTarget.h"
#include <iostream>
#include <iomanip>
#include <cmath>

// Evitar conflictos con macros max/min de Windows
#ifdef max
#undef max
#endif
#ifdef min
#undef min
#endif
#include <algorithm>

namespace D3D12Core {

    D3D12Core::D3D12Core() {
    }

    D3D12Core::~D3D12Core() {
        Shutdown();
    }

    bool D3D12Core::Initialize(HWND hwnd, UINT width, UINT height) {
        m_hwnd = hwnd;
        m_width = width;
        m_height = height;

        // Crear device
        m_device = std::make_unique<D3D12Device>();
        if (!m_device->Initialize()) {
            std::cerr << "Error: Failed to initialize D3D12 Device" << std::endl;
            return false;
        }

        // Crear command queue
        m_commandQueue = std::make_unique<D3D12CommandQueue>();
        if (!m_commandQueue->Initialize(m_device->GetDevice())) {
            std::cerr << "Error: Failed to initialize Command Queue" << std::endl;
            return false;
        }

        // Crear swap chain
        m_swapChain = std::make_unique<D3D12SwapChain>();
        if (!m_swapChain->Initialize(
            hwnd,
            m_device->GetFactory(),
            m_commandQueue->GetQueue(),
            width,
            height
        )) {
            std::cerr << "Error: Failed to initialize Swap Chain" << std::endl;
            return false;
        }

        // Crear render target views
        ID3D12Device* device = m_device->GetDevice();
        // Esto se hace internamente en CreateRenderTargetViews

        // Crear render target de alta resolución (1920x1080) para mantener calidad máxima
        // Nota: Por ahora está inicializado pero no se usa activamente debido a limitaciones de escalado
        // El sistema renderiza directamente al tamaño del viewport que se ajusta automáticamente
        m_highResRenderTarget = std::make_unique<D3D12HighResRenderTarget>();
        if (!m_highResRenderTarget->Initialize(device, m_renderWidth, m_renderHeight)) {
            std::cerr << "Error: Failed to initialize high-res render target" << std::endl;
            // No es crítico, continuamos sin él
            m_highResRenderTarget.reset();
        }

        std::wcout << L"DirectX 12 inicializado correctamente" << std::endl;
        std::wcout << L"Viewport: " << m_width << L"x" << m_height << L" (ajuste automático activo)" << std::endl;
        if (m_highResRenderTarget) {
            std::wcout << L"Render target de alta calidad disponible: " << m_renderWidth << L"x" << m_renderHeight << std::endl;
        }
        auto adapterInfo = m_device->GetAdapterInfo();
        std::wcout << L"Adaptador: " << adapterInfo.Description.c_str() << std::endl;
        std::wcout << L"Memoria de video dedicada: " << (adapterInfo.DedicatedVideoMemory / (1024ULL * 1024ULL * 1024ULL)) << L" GB" << std::endl;

        return true;
    }

    void D3D12Core::Shutdown() {
        if (m_commandQueue) {
            m_commandQueue->WaitForGPU();
        }

        m_highResRenderTarget.reset();
        m_swapChain.reset();
        m_commandQueue.reset();
        m_device.reset();
    }

    void D3D12Core::BeginFrame() {
        // Reset command list
        m_commandQueue->ResetCommandList();
        ID3D12GraphicsCommandList* commandList = m_commandQueue->GetCommandList();

        // Obtener back buffer actual (para uso posterior en EndFrame)
        m_currentBackBufferIndex = m_swapChain->GetCurrentBackBufferIndex();

        // Obtener back buffer actual
        ID3D12Resource* backBuffer = m_swapChain->GetCurrentBackBuffer();
        
        // Transición a render target
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = backBuffer;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &barrier);

        // Limpiar render target con color oscuro elegante
        D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_swapChain->GetCurrentRTV();
        const float clearColor[] = { 0.05f, 0.05f, 0.1f, 1.0f }; // Azul muy oscuro elegante
        commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

        // Establecer render target
        commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
        
        // Nota: El render target de alta resolución está temporalmente deshabilitado
        // porque CopyTextureRegion no puede hacer escalado real cuando los tamaños son diferentes.
        // Para implementar escalado de alta calidad, se necesita un pixel shader con filtrado bilinear.
        // Por ahora, renderizamos directamente al tamaño del viewport con máxima calidad posible.
    }

    void D3D12Core::EndFrame() {
        ID3D12GraphicsCommandList* commandList = m_commandQueue->GetCommandList();
        ID3D12Resource* backBuffer = m_swapChain->GetCurrentBackBuffer();

        // Transición del back buffer a PRESENT
        D3D12_RESOURCE_BARRIER barrier = {};
        barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
        barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
        barrier.Transition.pResource = backBuffer;
        barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
        barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
        barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
        commandList->ResourceBarrier(1, &barrier);

        // NO cerrar aquí - ExecuteCommandList ya lo hace
        // commandList->Close();

        // Ejecutar comandos (esto cierra el command list internamente)
        m_commandQueue->ExecuteCommandList();
    }

    void D3D12Core::Present() {
        if (!m_swapChain) {
            return; // Swap chain no válido
        }
        
        // Present retorna void, el manejo de errores está en D3D12SwapChain::Present()
        m_swapChain->Present(1, 0); // VSync habilitado
        
        m_commandQueue->Flush();
        m_frameIndex++;
    }

    void D3D12Core::Resize(UINT width, UINT height) {
        if (m_width == width && m_height == height) {
            return;
        }

        m_commandQueue->WaitForGPU();

        m_width = width;
        m_height = height;

        // Actualizar el swap chain al nuevo tamaño del viewport
        m_swapChain->Resize(width, height);
        
        // Sistema de resolución automática mejorado
        // Calcula la mejor resolución dentro del rango 800x600 a 1920x1080
        // basándose en el espacio disponible, aspect ratio y calidad
        
        // Resoluciones estándar optimizadas (ordenadas por calidad descendente)
        struct Resolution {
            UINT width;
            UINT height;
            double quality;
        };
        
        Resolution standardResolutions[] = {
            {1920, 1080, 1.0},   // Full HD (máxima calidad)
            {1600, 900, 0.89},   // 16:9
            {1440, 900, 0.83},   // 16:10
            {1366, 768, 0.76},   // WXGA
            {1280, 720, 0.71},   // HD
            {1152, 864, 0.64},   // XGA+
            {1024, 768, 0.58},   // XGA
            {800, 600, 0.46}     // SVGA (mínima calidad)
        };
        
        // Calcular aspect ratio del viewport
        double viewportAspectRatio = (double)width / (double)height;
        
        // Variables para la mejor resolución encontrada
        UINT optimalWidth = 800;
        UINT optimalHeight = 600;
        double bestScore = -1.0;
        
        // Buscar la mejor resolución estándar que se ajuste al espacio disponible
        for (const auto& res : standardResolutions) {
            // Verificar que la resolución quepa completamente en el espacio disponible
            if (res.width > width || res.height > height) {
                continue;
            }
            
            // Calcular el aspect ratio de esta resolución
            double resAspectRatio = (double)res.width / (double)res.height;
            
            // Calcular diferencia de aspect ratio (normalizada)
            double aspectRatioDiff = (resAspectRatio > viewportAspectRatio) ? 
                (resAspectRatio - viewportAspectRatio) : 
                (viewportAspectRatio - resAspectRatio);
            double aspectRatioMatch = 1.0 - (aspectRatioDiff / 0.5); // Normalizar a 0-1
            if (aspectRatioMatch < 0.0) aspectRatioMatch = 0.0;
            
            // Calcular uso del espacio disponible (qué porcentaje del viewport ocupa)
            double spaceUsage = ((double)res.width * (double)res.height) / ((double)width * (double)height);
            
            // Calcular píxeles totales (normalizado)
            double pixelCount = (double)(res.width * res.height) / (1920.0 * 1080.0);
            
            // Score combinado con pesos optimizados:
            // - 40% calidad de resolución estándar
            // - 30% match de aspect ratio
            // - 20% uso del espacio disponible
            // - 10% cantidad de píxeles (preferir más píxeles)
            double score = (res.quality * 0.4) + 
                          (aspectRatioMatch * 0.3) + 
                          (spaceUsage * 0.2) + 
                          (pixelCount * 0.1);
            
            if (score > bestScore) {
                bestScore = score;
                optimalWidth = res.width;
                optimalHeight = res.height;
            }
        }
        
        // Si encontramos una resolución estándar válida, usarla como base
        // pero ajustarla para aprovechar mejor el espacio disponible si es posible
        if (bestScore >= 0.0) {
            // Si el viewport es más grande que la resolución estándar encontrada,
            // intentar usar una resolución más alta que aún quepa en el espacio disponible
            // Esto maximiza la calidad mientras mantiene el aspect ratio correcto
            
            // Buscar si hay una resolución estándar más alta que aún quepa
            bool foundBetter = false;
            for (const auto& res : standardResolutions) {
                // Verificar que sea más grande que la actual y que quepa en el espacio
                if (res.width > optimalWidth && res.height > optimalHeight &&
                    res.width <= width && res.height <= height) {
                    // Verificar que el aspect ratio sea similar
                    double resAspectRatio = (double)res.width / (double)res.height;
                    double aspectDiff = (resAspectRatio > viewportAspectRatio) ? 
                        (resAspectRatio - viewportAspectRatio) : 
                        (viewportAspectRatio - resAspectRatio);
                    
                    if (aspectDiff < 0.15) { // Tolerancia de 15% para aspect ratio
                        optimalWidth = res.width;
                        optimalHeight = res.height;
                        foundBetter = true;
                        break;
                    }
                }
            }
            
            // Si no encontramos una mejor resolución estándar, intentar escalar la actual
            if (!foundBetter && width >= optimalWidth * 1.15 && height >= optimalHeight * 1.15) {
                // El viewport es significativamente más grande, podemos escalar hasta el tamaño del viewport
                // pero limitado al máximo de 1920x1080
                UINT maxScaleWidth = (width > 1920U) ? 1920U : width;
                UINT maxScaleHeight = (height > 1080U) ? 1080U : height;
                
                // Calcular escala manteniendo aspect ratio
                double scaleX = (double)maxScaleWidth / (double)optimalWidth;
                double scaleY = (double)maxScaleHeight / (double)optimalHeight;
                double scale = (scaleX < scaleY) ? scaleX : scaleY; // Usar el menor para mantener aspect ratio
                
                if (scale > 1.0) {
                    optimalWidth = (UINT)((double)optimalWidth * scale);
                    optimalHeight = (UINT)((double)optimalHeight * scale);
                    
                    // Asegurar que no exceda los límites
                    if (optimalWidth > 1920U) optimalWidth = 1920U;
                    if (optimalHeight > 1080U) optimalHeight = 1080U;
                }
            }
        } else {
            // No encontramos resolución estándar adecuada, usar el tamaño completo del viewport
            // pero limitado al rango permitido (800x600 a 1920x1080)
            optimalWidth = (width < 800U) ? 800U : ((width > 1920U) ? 1920U : width);
            optimalHeight = (height < 600U) ? 600U : ((height > 1080U) ? 1080U : height);
            
            // Ajustar para mantener aspect ratio del viewport si es necesario
            double customAspectRatio = (double)optimalWidth / (double)optimalHeight;
            double aspectDiff = (customAspectRatio > viewportAspectRatio) ? 
                (customAspectRatio - viewportAspectRatio) : 
                (viewportAspectRatio - customAspectRatio);
            
            if (aspectDiff > 0.05) { // Tolerancia de 5% para aspect ratio
                if (customAspectRatio > viewportAspectRatio) {
                    optimalWidth = (UINT)((double)optimalHeight * viewportAspectRatio);
                } else {
                    optimalHeight = (UINT)((double)optimalWidth / viewportAspectRatio);
                }
                
                // Asegurar que esté dentro del rango usando comparaciones directas
                optimalWidth = (optimalWidth < 800U) ? 800U : ((optimalWidth > 1920U) ? 1920U : optimalWidth);
                optimalHeight = (optimalHeight < 600U) ? 600U : ((optimalHeight > 1080U) ? 1080U : optimalHeight);
            }
        }
        
        // Actualizar la resolución de renderizado si cambió
        // IMPORTANTE: Esta resolución se usa para calidad interna, pero el viewport visual
        // siempre usa el tamaño completo (width x height) para llenar todo el espacio
        if (m_renderWidth != optimalWidth || m_renderHeight != optimalHeight) {
            m_renderWidth = optimalWidth;
            m_renderHeight = optimalHeight;
            
            // Log informativo (solo cuando cambia la resolución)
            std::wcout << L"[Resolución Automática] Viewport: " << width << L"x" << height 
                      << L" | Calidad interna: " << m_renderWidth << L"x" << m_renderHeight << std::endl;
        }
    }

} // namespace D3D12Core

