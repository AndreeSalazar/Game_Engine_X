#include "D3D12Device.h"
#include <dxgi.h>
#include <iostream>

#ifdef _DEBUG
#include <dxgidebug.h>
#endif

namespace D3D12Core {

    D3D12Device::D3D12Device() {
    }

    D3D12Device::~D3D12Device() {
        Shutdown();
    }

    bool D3D12Device::Initialize(bool enableDebugLayer) {
        if (enableDebugLayer) {
            ComPtr<ID3D12Debug> debugController;
            if (SUCCEEDED(D3D12GetDebugInterface(IID_PPV_ARGS(&debugController)))) {
                debugController->EnableDebugLayer();
            }
        }

        if (!CreateFactory()) return false;
        if (!SelectAdapter()) return false;
        if (!CreateDevice()) return false;

        return true;
    }

    void D3D12Device::Shutdown() {
        m_device.Reset();
        m_adapter.Reset();
        m_factory.Reset();
    }

    bool D3D12Device::CreateFactory() {
        UINT factoryFlags = 0;
#ifdef _DEBUG
        factoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
#endif

        HRESULT hr = CreateDXGIFactory2(factoryFlags, IID_PPV_ARGS(&m_factory));
        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create DXGI Factory" << std::endl;
            return false;
        }

        return true;
    }

    bool D3D12Device::SelectAdapter() {
        ComPtr<IDXGIAdapter1> adapter;
        ComPtr<IDXGIAdapter1> bestAdapter;
        SIZE_T maxDedicatedVideoMemory = 0;

        // Buscar el adaptador con más memoria dedicada (RTX 3060 12GB)
        for (UINT i = 0; m_factory->EnumAdapters1(i, &adapter) != DXGI_ERROR_NOT_FOUND; ++i) {
            DXGI_ADAPTER_DESC1 desc;
            adapter->GetDesc1(&desc);

            // Preferir adaptadores con más memoria dedicada
            if (desc.DedicatedVideoMemory > maxDedicatedVideoMemory) {
                maxDedicatedVideoMemory = desc.DedicatedVideoMemory;
                bestAdapter = adapter;
                
                // Guardar información del adaptador
                m_adapterInfo.Description = desc.Description;
                m_adapterInfo.DedicatedVideoMemory = desc.DedicatedVideoMemory;
                m_adapterInfo.DedicatedSystemMemory = desc.DedicatedSystemMemory;
                m_adapterInfo.SharedSystemMemory = desc.SharedSystemMemory;
            }
        }

        if (!bestAdapter) {
            std::cerr << "Error: No suitable adapter found" << std::endl;
            return false;
        }

        m_adapter = bestAdapter;
        return true;
    }

    bool D3D12Device::CreateDevice() {
        HRESULT hr = D3D12CreateDevice(
            m_adapter.Get(),
            D3D_FEATURE_LEVEL_12_0,
            IID_PPV_ARGS(&m_device)
        );

        if (FAILED(hr)) {
            std::cerr << "Error: Failed to create D3D12 Device" << std::endl;
            return false;
        }

        return true;
    }

} // namespace D3D12Core

