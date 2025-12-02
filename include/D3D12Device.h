#pragma once

#include "D3D12Core.h"
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl/client.h>

namespace D3D12Core {

    class D3D12Device {
    public:
        D3D12Device();
        ~D3D12Device();

        bool Initialize(bool enableDebugLayer = true);
        void Shutdown();

        ID3D12Device* GetDevice() const { return m_device.Get(); }
        IDXGIFactory4* GetFactory() const { return m_factory.Get(); }

        // Información del adaptador (RTX 3060)
        struct AdapterInfo {
            std::wstring Description;
            UINT64 DedicatedVideoMemory;
            UINT64 DedicatedSystemMemory;
            UINT64 SharedSystemMemory;
        };
        AdapterInfo GetAdapterInfo() const { return m_adapterInfo; }

    private:
        ComPtr<IDXGIFactory4> m_factory;
        ComPtr<IDXGIAdapter1> m_adapter;
        ComPtr<ID3D12Device> m_device;
        AdapterInfo m_adapterInfo;

        bool CreateFactory();
        bool SelectAdapter();
        bool CreateDevice();
    };

} // namespace D3D12Core

