#include "../include/D3D12Core.h"
#include "../include/D3D12Device.h"
#include "../include/D3D12CommandQueue.h"
#include "../include/D3D12PipelineState.h"
#include "../include/D3D12Mesh.h"
#include "../include/D3D12ConstantBuffer.h"
#include "../include/Shader.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <DirectXMath.h>
#include <string>

using namespace DirectX;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Abrir consola para ver errores
    AllocConsole();
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    FILE* pCerr;
    freopen_s(&pCerr, "CONOUT$", "w", stderr);

    std::cout << "=== Iniciando DirectX 12 Test ===" << std::endl;

    // Registrar clase de ventana
    const wchar_t CLASS_NAME[] = L"DirectX12WindowClass";

    WNDCLASS wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    RegisterClass(&wc);

    // Crear ventana
    const UINT width = 1280;
    const UINT height = 720;

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DirectX 12 - RTX 3060 12GB",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT,
        width, height,
        nullptr,
        nullptr,
        hInstance,
        nullptr
    );

    if (hwnd == nullptr) {
        std::cerr << "Error: Failed to create window" << std::endl;
        FreeConsole();
        return 1;
    }

    ShowWindow(hwnd, nCmdShow);
    UpdateWindow(hwnd);

    std::cout << "Ventana creada correctamente" << std::endl;

    // Inicializar DirectX 12
    std::cout << "Inicializando DirectX 12..." << std::endl;
    D3D12Core::D3D12Core* d3d12 = new D3D12Core::D3D12Core();
    if (!d3d12->Initialize(hwnd, width, height)) {
        std::cerr << "Error: Failed to initialize DirectX 12" << std::endl;
        delete d3d12;
        FreeConsole();
        return 1;
    }
    std::cout << "DirectX 12 inicializado correctamente" << std::endl;

    // Compilar shaders
    std::cout << "Compilando shaders..." << std::endl;
    D3D12Core::Shader vertexShader, pixelShader;
    std::vector<BYTE> vsBytecode, psBytecode;
    std::string error;

    // Intentar compilar shaders desde diferentes ubicaciones
    bool vsCompiled = false;
    bool psCompiled = false;

    // Intentar desde el directorio del proyecto
    std::wcout << L"Intentando compilar: shaders/BasicVS.hlsl" << std::endl;
    if (D3D12Core::ShaderCompiler::CompileShader(L"shaders/BasicVS.hlsl", "main", "vs_5_0", vsBytecode, error)) {
        vsCompiled = true;
        std::cout << "Vertex shader compilado correctamente" << std::endl;
    } else {
        // Intentar desde el directorio del ejecutable
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        std::wstring exeDir = exePath;
        size_t lastSlash = exeDir.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            exeDir = exeDir.substr(0, lastSlash + 1);
            std::wstring vsPath = exeDir + L"shaders\\BasicVS.hlsl";
            std::wcout << L"Intentando compilar: " << vsPath << std::endl;
            if (D3D12Core::ShaderCompiler::CompileShader(vsPath, "main", "vs_5_0", vsBytecode, error)) {
                vsCompiled = true;
                std::cout << "Vertex shader compilado correctamente" << std::endl;
            }
        }
    }

    if (!vsCompiled) {
        std::cerr << "Error compiling vertex shader: " << error << std::endl;
        std::cerr << "Asegurate de que shaders/BasicVS.hlsl existe" << std::endl;
        delete d3d12;
        FreeConsole();
        return 1;
    }

    // Compilar pixel shader
    std::wcout << L"Intentando compilar: shaders/BasicPS.hlsl" << std::endl;
    if (D3D12Core::ShaderCompiler::CompileShader(L"shaders/BasicPS.hlsl", "main", "ps_5_0", psBytecode, error)) {
        psCompiled = true;
        std::cout << "Pixel shader compilado correctamente" << std::endl;
    } else {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        std::wstring exeDir = exePath;
        size_t lastSlash = exeDir.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            exeDir = exeDir.substr(0, lastSlash + 1);
            std::wstring psPath = exeDir + L"shaders\\BasicPS.hlsl";
            std::wcout << L"Intentando compilar: " << psPath << std::endl;
            if (D3D12Core::ShaderCompiler::CompileShader(psPath, "main", "ps_5_0", psBytecode, error)) {
                psCompiled = true;
                std::cout << "Pixel shader compilado correctamente" << std::endl;
            }
        }
    }

    if (!psCompiled) {
        std::cerr << "Error compiling pixel shader: " << error << std::endl;
        std::cerr << "Asegurate de que shaders/BasicPS.hlsl existe" << std::endl;
        delete d3d12;
        FreeConsole();
        return 1;
    }

    vertexShader.LoadFromBytecode(vsBytecode.data(), vsBytecode.size());
    pixelShader.LoadFromBytecode(psBytecode.data(), psBytecode.size());

    // Crear Pipeline State
    std::cout << "Creando Pipeline State..." << std::endl;
    D3D12Core::D3D12PipelineState* pso = new D3D12Core::D3D12PipelineState();
    if (!pso->Initialize(
        d3d12->GetDevice()->GetDevice(),
        vertexShader,
        pixelShader,
        D3D12Core::BACK_BUFFER_FORMAT)) {
        std::cerr << "Error: Failed to create pipeline state" << std::endl;
        delete pso;
        delete d3d12;
        FreeConsole();
        return 1;
    }
    std::cout << "Pipeline State creado correctamente" << std::endl;

    // Crear geometría del cubo (estilo Vulkan Cube)
    std::cout << "Creando geometria del cubo..." << std::endl;
    std::vector<D3D12Core::Vertex> cubeVertices = {
        // Cara frontal (Z+) - Colores más vibrantes
        {{-1.0f, -1.0f,  1.0f}, {1.0f, 0.2f, 0.2f}}, // Rojo brillante
        {{ 1.0f, -1.0f,  1.0f}, {0.2f, 1.0f, 0.2f}}, // Verde brillante
        {{ 1.0f,  1.0f,  1.0f}, {0.2f, 0.2f, 1.0f}}, // Azul brillante
        {{-1.0f,  1.0f,  1.0f}, {1.0f, 1.0f, 0.2f}}, // Amarillo brillante
        
        // Cara trasera (Z-)
        {{ 1.0f, -1.0f, -1.0f}, {1.0f, 0.2f, 1.0f}}, // Magenta brillante
        {{-1.0f, -1.0f, -1.0f}, {0.2f, 1.0f, 1.0f}}, // Cyan brillante
        {{-1.0f,  1.0f, -1.0f}, {1.0f, 1.0f, 1.0f}}, // Blanco puro
        {{ 1.0f,  1.0f, -1.0f}, {0.8f, 0.8f, 0.8f}}  // Gris claro
    };

    std::vector<UINT> cubeIndices = {
        // Cara frontal
        0, 1, 2,  2, 3, 0,
        // Cara trasera
        4, 5, 6,  6, 7, 4,
        // Cara izquierda
        5, 0, 3,  3, 6, 5,
        // Cara derecha
        1, 4, 7,  7, 2, 1,
        // Cara superior
        3, 2, 7,  7, 6, 3,
        // Cara inferior
        5, 4, 1,  1, 0, 5
    };

    // Crear mesh del cubo
    D3D12Core::D3D12Mesh* cubeMesh = new D3D12Core::D3D12Mesh();
    if (!cubeMesh->Initialize(
        d3d12->GetDevice()->GetDevice(),
        d3d12->GetCommandQueue()->GetQueue(),
        cubeVertices,
        cubeIndices)) {
        std::cerr << "Error: Failed to create cube mesh" << std::endl;
        delete cubeMesh;
        delete pso;
        delete d3d12;
        FreeConsole();
        return 1;
    }
    std::cout << "Mesh del cubo creado correctamente" << std::endl;

    // Crear constant buffer para matrices MVP
    std::cout << "Creando Constant Buffer..." << std::endl;
    D3D12Core::D3D12ConstantBuffer* mvpBuffer = new D3D12Core::D3D12ConstantBuffer();
    if (!mvpBuffer->Initialize(d3d12->GetDevice()->GetDevice(), sizeof(D3D12Core::MVPConstantBuffer))) {
        std::cerr << "Error: Failed to create MVP constant buffer" << std::endl;
        delete mvpBuffer;
        delete cubeMesh;
        delete pso;
        delete d3d12;
        FreeConsole();
        return 1;
    }
    std::cout << "Constant Buffer creado correctamente" << std::endl;

    // Guardar punteros en la ventana (usando estructura)
    struct AppData {
        D3D12Core::D3D12Core* d3d12;
        D3D12Core::D3D12PipelineState* pso;
        D3D12Core::D3D12Mesh* mesh;
        D3D12Core::D3D12ConstantBuffer* mvpBuffer;
        float rotationAngle = 0.0f;
        UINT width;
        UINT height;
    };
    AppData* appData = new AppData{ d3d12, pso, cubeMesh, mvpBuffer, 0.0f, width, height };
    SetWindowLongPtr(hwnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(appData));

    std::cout << "=== Iniciando loop de renderizado ===" << std::endl;
    std::cout << "Presiona cualquier tecla en la consola para salir..." << std::endl;
    std::cout << "AppData width: " << width << ", height: " << height << std::endl;
    std::cout << "AppData pointer: " << appData << std::endl;
    std::cout << "HWND: " << hwnd << std::endl;

    // Loop principal
    MSG msg = {};
    bool running = true;
    int loopIterations = 0;
    int messageCount = 0;

    std::cout << "Entrando al loop principal..." << std::endl;
    std::cout.flush(); // Forzar flush

    while (running) {
        std::cout << "[DEBUG] Inicio de iteracion del while..." << std::endl;
        std::cout.flush();
        
        // Procesar mensajes de Windows (NO bloqueante)
        BOOL hasMessage = PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE);
        if (hasMessage) {
            messageCount++;
            std::cout << "[DEBUG] Mensaje recibido: " << msg.message << std::endl;
            std::cout.flush();
            
            if (msg.message == WM_QUIT) {
                std::cout << "WM_QUIT recibido, saliendo..." << std::endl;
                running = false;
                break;
            }
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        // Renderizar frame
        std::cout << "[DEBUG] Verificando running: " << (running ? "true" : "false") << std::endl;
        std::cout.flush();
        
        if (running) {
            loopIterations++;
            std::cout << "[DEBUG] Loop iteration: " << loopIterations << std::endl;
            std::cout.flush();
            
            if (loopIterations == 1) {
                std::cout << "=== PRIMERA ITERACION DEL LOOP ===" << std::endl;
                std::cout << "Mensajes procesados hasta ahora: " << messageCount << std::endl;
                std::cout << "AppData: " << (appData ? "OK" : "NULL") << std::endl;
                if (appData) {
                    std::cout << "AppData width: " << appData->width << ", height: " << appData->height << std::endl;
                }
                std::cout.flush();
            }
            
            // Verificar que appData no sea null antes de usarlo
            if (!appData) {
                std::cerr << "ERROR: appData es NULL!" << std::endl;
                break;
            }
            std::cout << "[DEBUG] Antes de actualizar rotacion..." << std::endl;
            std::cout.flush();
            
            // Actualizar rotación - más suave y rápida
            appData->rotationAngle += 0.015f; // Rotación más rápida
            
            std::cout << "[DEBUG] Despues de actualizar rotacion..." << std::endl;
            std::cout.flush();
            if (appData->rotationAngle > XM_2PI) {
                appData->rotationAngle -= XM_2PI;
            }

            // Calcular matrices MVP
            // Model: Rotación en múltiples ejes para efecto más interesante
            XMMATRIX scale = XMMatrixScaling(0.6f, 0.6f, 0.6f); // Tamaño ligeramente mayor
            XMMATRIX rotationY = XMMatrixRotationY(appData->rotationAngle);
            XMMATRIX rotationX = XMMatrixRotationX(appData->rotationAngle * 0.7f); // Rotación en X más lenta
            XMMATRIX model = scale * rotationX * rotationY; // Combinar rotaciones
            
            // View: Cámara con ligero ángulo para mejor perspectiva
            XMVECTOR eye = XMVectorSet(0.0f, 1.0f, -4.0f, 1.0f); // Ligeramente arriba y más atrás
            XMVECTOR focus = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
            XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
            XMMATRIX view = XMMatrixLookAtLH(eye, focus, up);
            
            // Projection: Perspectiva con FOV más amplio para ver mejor
            float aspectRatio = (float)appData->width / (float)appData->height;
            if (aspectRatio <= 0.0f) aspectRatio = 1.0f; // Evitar división por cero
            XMMATRIX projection = XMMatrixPerspectiveFovLH(XM_PIDIV4, aspectRatio, 0.1f, 100.0f);

            // Actualizar constant buffer
            // Transponer y almacenar matrices (DirectX usa matrices en formato row-major)
            D3D12Core::MVPConstantBuffer mvpData;
            XMStoreFloat4x4(&mvpData.model, XMMatrixTranspose(model));
            XMStoreFloat4x4(&mvpData.view, XMMatrixTranspose(view));
            XMStoreFloat4x4(&mvpData.projection, XMMatrixTranspose(projection));
            mvpBuffer->UpdateData(&mvpData, sizeof(D3D12Core::MVPConstantBuffer));

            // Render frame
            d3d12->BeginFrame();
            
            // Obtener command list
            ID3D12GraphicsCommandList* commandList = d3d12->GetCommandQueue()->GetCommandList();
            
            // Establecer pipeline state PRIMERO
            commandList->SetPipelineState(pso->GetPSO());
            commandList->SetGraphicsRootSignature(pso->GetRootSignature());
            
            // Establecer viewport y scissor rect ANTES de bindear recursos
            D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)appData->width, (float)appData->height, 0.0f, 1.0f };
            D3D12_RECT scissorRect = { 0, 0, (LONG)appData->width, (LONG)appData->height };
            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissorRect);
            
            // Bind constant buffer
            if (pso->HasConstantBuffer()) {
                mvpBuffer->Bind(commandList, 0);
            }
            
            // Debug: Verificar valores
            static bool firstFrame = true;
            if (firstFrame) {
                std::cout << "=== DEBUG PRIMER FRAME ===" << std::endl;
                std::cout << "Width: " << appData->width << ", Height: " << appData->height << std::endl;
                std::cout << "Viewport: " << viewport.Width << "x" << viewport.Height << std::endl;
                std::cout << "Rotation angle: " << appData->rotationAngle << std::endl;
                std::cout << "PSO: " << (pso->GetPSO() ? "OK" : "NULL") << std::endl;
                std::cout << "Root Signature: " << (pso->GetRootSignature() ? "OK" : "NULL") << std::endl;
                std::cout << "Constant Buffer: " << (mvpBuffer ? "OK" : "NULL") << std::endl;
                std::cout << "Command List: " << (commandList ? "OK" : "NULL") << std::endl;
                std::cout << "D3D12 Core: " << (d3d12 ? "OK" : "NULL") << std::endl;
                firstFrame = false;
            }
            
            // Dibujar el cubo
            cubeMesh->Draw(commandList);
            
            d3d12->EndFrame();
            d3d12->Present();
            
            // Control de FPS (60 FPS)
            Sleep(16);
        }
        
        // Si no hay mensajes, seguir renderizando
        if (!PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE)) {
            // No hay mensajes pendientes, continuar renderizando
        }
    }
    
    std::cout << "Loop terminado. Total iteraciones: " << loopIterations << std::endl;

    std::cout << "=== Limpiando recursos ===" << std::endl;

    // Limpiar
    delete mvpBuffer;
    delete cubeMesh;
    delete pso;
    delete d3d12;
    delete appData;
    UnregisterClass(CLASS_NAME, hInstance);

    FreeConsole();
    return 0;
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    struct AppData {
        D3D12Core::D3D12Core* d3d12;
        D3D12Core::D3D12PipelineState* pso;
        D3D12Core::D3D12Mesh* mesh;
        D3D12Core::D3D12ConstantBuffer* mvpBuffer;
        float rotationAngle;
        UINT width;
        UINT height;
    };
    
    AppData* appData = reinterpret_cast<AppData*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA)
    );
    
    D3D12Core::D3D12Core* d3d12 = appData ? appData->d3d12 : nullptr;

    switch (uMsg) {
    case WM_DESTROY:
        PostQuitMessage(0);
        return 0;

    case WM_SIZE:
        if (d3d12 && appData && wParam != SIZE_MINIMIZED) {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            appData->width = width;
            appData->height = height;
            d3d12->Resize(width, height);
        }
        return 0;

    case WM_PAINT:
        // El renderizado se hace en el loop principal
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
