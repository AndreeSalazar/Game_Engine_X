#include "D3D12Core.h"
#include "D3D12Device.h"
#include "D3D12CommandQueue.h"
#include "D3D12PipelineState.h"
#include "D3D12Mesh.h"
#include "D3D12ConstantBuffer.h"
#include "D3D12Material.h"
#include "Shader.h"
#include <windows.h>
#include <iostream>
#include <vector>
#include <cmath>
#include <DirectXMath.h>
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

#ifndef WS_CHILD
#define WS_CHILD 0x40000000L
#endif

using namespace DirectX;

// Forward declarations
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Estructura de configuración
struct CubeConfig {
    float rotationSpeed = 0.015f;
    float scale = 0.6f;
    float rotationXMultiplier = 0.7f;
    float cameraX = 0.0f;
    float cameraY = 1.0f;
    float cameraZ = -4.0f;
    float fov = XM_PIDIV4;
    float clearColorR = 0.05f;
    float clearColorG = 0.05f;
    float clearColorB = 0.1f;
    bool autoRotate = true;
};

// Función simple para leer config.json (parsing básico sin librerías externas)
bool LoadConfig(CubeConfig& config) {
    // Buscar config.json en múltiples ubicaciones
    std::ifstream file;
    std::string configPaths[] = {
        "Engine/Binaries/Win64/config.json",
        "x64/Debug/config.json",
        "config.json"
    };
    
    bool fileOpened = false;
    for (const auto& path : configPaths) {
        file.open(path);
        if (file.is_open()) {
            fileOpened = true;
            break;
        }
    }
    
    if (!fileOpened || !file.is_open()) {
        return false; // Archivo no existe, usar valores por defecto
    }
    
    std::string line;
    while (std::getline(file, line)) {
        // Buscar valores en formato simple: "rotationSpeed": 0.02,
        if (line.find("\"rotationSpeed\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                // Eliminar espacios y comas
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.rotationSpeed = std::stof(value);
            }
        }
        else if (line.find("\"scale\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.scale = std::stof(value);
            }
        }
        else if (line.find("\"rotationXMultiplier\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.rotationXMultiplier = std::stof(value);
            }
        }
        else if (line.find("\"cameraX\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.cameraX = std::stof(value);
            }
        }
        else if (line.find("\"cameraY\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.cameraY = std::stof(value);
            }
        }
        else if (line.find("\"cameraZ\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.cameraZ = std::stof(value);
            }
        }
        else if (line.find("\"fov\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.fov = std::stof(value);
            }
        }
        else if (line.find("\"autoRotate\"") != std::string::npos) {
            size_t colon = line.find(':');
            if (colon != std::string::npos) {
                std::string value = line.substr(colon + 1);
                value.erase(0, value.find_first_not_of(" \t,"));
                value.erase(value.find_last_not_of(" \t,") + 1);
                config.autoRotate = (value.find("true") != std::string::npos);
            }
        }
    }
    
    file.close();
    return true;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Abrir consola para ver errores
    AllocConsole();
    FILE* pCout;
    freopen_s(&pCout, "CONOUT$", "w", stdout);
    FILE* pCerr;
    freopen_s(&pCerr, "CONOUT$", "w", stderr);

    std::cout << "=== Iniciando DirectX 12 Test ===" << std::endl;

    // Parsear argumentos de línea de comandos para obtener HWND padre
    HWND parentHwnd = nullptr;
    if (lpCmdLine != nullptr && strlen(lpCmdLine) > 0) {
        std::string cmdLine(lpCmdLine);
        size_t pos = cmdLine.find("--parent-hwnd");
        if (pos != std::string::npos) {
            size_t start = cmdLine.find_first_of("0123456789", pos);
            if (start != std::string::npos) {
                try {
                    long long hwndValue = std::stoll(cmdLine.substr(start));
                    parentHwnd = reinterpret_cast<HWND>(hwndValue);
                    std::cout << "HWND padre recibido: " << parentHwnd << std::endl;
                }
                catch (...) {
                    std::cerr << "Error al parsear HWND padre" << std::endl;
                }
            }
        }
    }

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
    UINT width = 1280;
    UINT height = 720;

    // Si hay un padre, obtener el tamaño del contenedor padre
    if (parentHwnd) {
        RECT parentRect;
        if (GetClientRect(parentHwnd, &parentRect)) {
            width = parentRect.right - parentRect.left;
            height = parentRect.bottom - parentRect.top;
            // Asegurar tamaño mínimo válido
            if (width <= 0) width = 800;
            if (height <= 0) height = 600;
            std::cout << "Tamaño inicial obtenido del padre: " << width << "x" << height << std::endl;
        }
    }

    // Si hay un padre, crear como ventana hijo, sino como ventana independiente
    DWORD dwStyle = parentHwnd ? (WS_CHILD | WS_VISIBLE) : WS_OVERLAPPEDWINDOW;

    HWND hwnd = CreateWindowEx(
        0,
        CLASS_NAME,
        L"DirectX 12 - RTX 3060 12GB",
        dwStyle,
        parentHwnd ? 0 : CW_USEDEFAULT,
        parentHwnd ? 0 : CW_USEDEFAULT,
        width, height,
        parentHwnd, // HWND padre si existe
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
    std::wcout << L"Intentando compilar: Engine/Rendering/Shaders/BasicVS.hlsl" << std::endl;
    if (D3D12Core::ShaderCompiler::CompileShader(L"Engine/Rendering/Shaders/BasicVS.hlsl", "main", "vs_5_0", vsBytecode, error)) {
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
            std::wstring vsPath = exeDir + L"Shaders\\BasicVS.hlsl";
            std::wcout << L"Intentando compilar: " << vsPath << std::endl;
            if (D3D12Core::ShaderCompiler::CompileShader(vsPath, "main", "vs_5_0", vsBytecode, error)) {
                vsCompiled = true;
                std::cout << "Vertex shader compilado correctamente" << std::endl;
            }
        }
    }

    if (!vsCompiled) {
        std::cerr << "Error compiling vertex shader: " << error << std::endl;
        std::cerr << "Asegurate de que Engine/Rendering/Shaders/BasicVS.hlsl existe" << std::endl;
        delete d3d12;
        FreeConsole();
        return 1;
    }

    // Compilar pixel shader
    std::wcout << L"Intentando compilar: Engine/Rendering/Shaders/BasicPS.hlsl" << std::endl;
    if (D3D12Core::ShaderCompiler::CompileShader(L"Engine/Rendering/Shaders/BasicPS.hlsl", "main", "ps_5_0", psBytecode, error)) {
        psCompiled = true;
        std::cout << "Pixel shader compilado correctamente" << std::endl;
    } else {
        wchar_t exePath[MAX_PATH];
        GetModuleFileNameW(nullptr, exePath, MAX_PATH);
        std::wstring exeDir = exePath;
        size_t lastSlash = exeDir.find_last_of(L"\\/");
        if (lastSlash != std::wstring::npos) {
            exeDir = exeDir.substr(0, lastSlash + 1);
            std::wstring psPath = exeDir + L"Shaders\\BasicPS.hlsl";
            std::wcout << L"Intentando compilar: " << psPath << std::endl;
            if (D3D12Core::ShaderCompiler::CompileShader(psPath, "main", "ps_5_0", psBytecode, error)) {
                psCompiled = true;
                std::cout << "Pixel shader compilado correctamente" << std::endl;
            }
        }
    }

    if (!psCompiled) {
        std::cerr << "Error compiling pixel shader: " << error << std::endl;
        std::cerr << "Asegurate de que Engine/Rendering/Shaders/BasicPS.hlsl existe" << std::endl;
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
        D3D12Core::D3D12Material* material; // Material System
        D3D12Core::D3D12Mesh* mesh;
        D3D12Core::D3D12ConstantBuffer* mvpBuffer;
        float rotationAngle = 0.0f;
        UINT width;
        UINT height;
        CubeConfig config; // Configuración desde C#
    };
    
    // Crear Material System
    std::cout << "Creando Material System..." << std::endl;
    D3D12Core::D3D12Material* material = new D3D12Core::D3D12Material();
    std::string vsPath = "Engine/Rendering/Shaders/BasicVS.hlsl";
    std::string psPath = "Engine/Rendering/Shaders/BasicPS.hlsl";
    
    bool materialInitialized = material->Initialize(
        d3d12->GetDevice()->GetDevice(), 
        "CubeMaterial", 
        vsPath, 
        psPath
    );
    
    if (materialInitialized) {
        std::cout << "Material System inicializado correctamente" << std::endl;
    } else {
        std::cout << "Advertencia: Material System no inicializado, usando PSO básico" << std::endl;
    }
    
    // Cargar configuración inicial
    CubeConfig initialConfig;
    LoadConfig(initialConfig);
    
    AppData* appData = new AppData{ d3d12, pso, material, cubeMesh, mvpBuffer, 0.0f, width, height, initialConfig };
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

    // Variables para optimizar carga de configuración
    static std::filesystem::file_time_type lastConfigWriteTime;
    static bool configFileExists = false;
    
    // Control de frame time para VSync
    LARGE_INTEGER frequency, lastTime, currentTime;
    QueryPerformanceFrequency(&frequency);
    QueryPerformanceCounter(&lastTime);
    const double targetFrameTime = 1.0 / 60.0; // 60 FPS
    
    // Contador de frames para debugging
    static int frameCount = 0;
    static int lastLogFrame = 0;
    
    std::cout << "=== Loop iniciado, renderizando continuamente ===" << std::endl;
    std::cout << "Presiona ESC en la ventana o cierra la ventana para salir" << std::endl;
    
    while (running) {
        frameCount++;
        
        // Procesar mensajes de Windows (NO bloqueante)
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
            if (msg.message == WM_QUIT) {
                std::cout << "WM_QUIT recibido, saliendo del loop..." << std::endl;
                running = false;
                break;
            }
            
            // Manejar tecla ESC para salir
            if (msg.message == WM_KEYDOWN && msg.wParam == VK_ESCAPE) {
                std::cout << "ESC presionado, saliendo del loop..." << std::endl;
                running = false;
                PostQuitMessage(0);
                break;
            }
            
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        
        if (!running) {
            std::cout << "Running = false, saliendo del loop..." << std::endl;
            break;
        }
        
        // Verificar que appData no sea null antes de usarlo
        if (!appData) {
            std::cerr << "ERROR: appData es NULL! Reintentando..." << std::endl;
            // Reintentar obtener appData desde la ventana
            appData = reinterpret_cast<AppData*>(GetWindowLongPtr(hwnd, GWLP_USERDATA));
            if (!appData) {
                std::cerr << "ERROR: No se pudo recuperar appData, saltando frame" << std::endl;
                Sleep(100); // Esperar un poco antes de reintentar
                continue; // Continuar el loop en lugar de detenerlo
            }
        }
        
        // Cargar configuración desde C# solo si el archivo cambió (optimización)
        bool configChanged = false;
        std::string configPath = "Engine/Binaries/Win64/config.json";
        if (std::filesystem::exists(configPath)) {
            auto writeTime = std::filesystem::last_write_time(configPath);
            if (!configFileExists || writeTime != lastConfigWriteTime) {
                LoadConfig(appData->config);
                lastConfigWriteTime = writeTime;
                configFileExists = true;
                configChanged = true;
            }
        } else {
            // Si no existe, intentar cargar desde otras ubicaciones cada cierto tiempo
            static int configCheckCounter = 0;
            if (configCheckCounter++ % 60 == 0) { // Cada 60 frames (~1 segundo)
                LoadConfig(appData->config);
            }
        }
        
        // Cargar material desde Material Editor (solo si cambió, optimización)
        static std::filesystem::file_time_type lastMaterialWriteTime;
        if (appData->material && appData->material->IsValid()) {
            std::string materialPath = "Engine/Binaries/Win64/current_material.json";
            if (std::filesystem::exists(materialPath)) {
                auto writeTime = std::filesystem::last_write_time(materialPath);
                if (writeTime != lastMaterialWriteTime) {
                    std::ifstream matFile(materialPath);
                    if (matFile.is_open()) {
                        std::string json((std::istreambuf_iterator<char>(matFile)),
                                         std::istreambuf_iterator<char>());
                        matFile.close();
                        
                        // Actualizar parámetros del material desde JSON
                        if (json.find("\"BaseColor\"") != std::string::npos) {
                            size_t start = json.find("\"BaseColor\"");
                            if (start != std::string::npos) {
                                size_t valueStart = json.find('[', start);
                                size_t valueEnd = json.find(']', valueStart);
                                if (valueStart != std::string::npos && valueEnd != std::string::npos) {
                                    std::string values = json.substr(valueStart + 1, valueEnd - valueStart - 1);
                                    std::istringstream valueStream(values);
                                    float r, g, b;
                                    char comma;
                                    if (valueStream >> r >> comma >> g >> comma >> b) {
                                        appData->material->SetVector3("BaseColor", DirectX::XMFLOAT3(r, g, b));
                                    }
                                }
                            }
                        }
                        lastMaterialWriteTime = writeTime;
                    }
                }
            }
        }
        
        // Actualizar rotación según configuración
        if (appData->config.autoRotate) {
            appData->rotationAngle += appData->config.rotationSpeed;
        }
        if (appData->rotationAngle > XM_2PI) {
            appData->rotationAngle -= XM_2PI;
        }

        // Calcular matrices MVP usando configuración
        // Model: Rotación en múltiples ejes usando configuración
        XMMATRIX scale = XMMatrixScaling(
            appData->config.scale, 
            appData->config.scale, 
            appData->config.scale
        );
        XMMATRIX rotationY = XMMatrixRotationY(appData->rotationAngle);
        XMMATRIX rotationX = XMMatrixRotationX(appData->rotationAngle * appData->config.rotationXMultiplier);
        XMMATRIX model = scale * rotationX * rotationY; // Combinar rotaciones
        
        // View: Cámara usando configuración desde C#
        XMVECTOR eye = XMVectorSet(
            appData->config.cameraX, 
            appData->config.cameraY, 
            appData->config.cameraZ, 
            1.0f
        );
        XMVECTOR focus = XMVectorSet(0.0f, 0.0f, 0.0f, 1.0f);
        XMVECTOR up = XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
        XMMATRIX view = XMMatrixLookAtLH(eye, focus, up);
        
        // Projection: FOV desde configuración
        float aspectRatio = (float)appData->width / (float)appData->height;
        if (aspectRatio <= 0.0f) aspectRatio = 1.0f; // Evitar división por cero
        XMMATRIX projection = XMMatrixPerspectiveFovLH(appData->config.fov, aspectRatio, 0.1f, 100.0f);

        // Actualizar constant buffer
        // Transponer y almacenar matrices (DirectX usa matrices en formato row-major)
        D3D12Core::MVPConstantBuffer mvpData;
        XMStoreFloat4x4(&mvpData.model, XMMatrixTranspose(model));
        XMStoreFloat4x4(&mvpData.view, XMMatrixTranspose(view));
        XMStoreFloat4x4(&mvpData.projection, XMMatrixTranspose(projection));
        mvpBuffer->UpdateData(&mvpData, sizeof(D3D12Core::MVPConstantBuffer));

        // Render frame (con manejo de errores robusto para que el loop continúe)
        try {
            // Verificar que d3d12 esté válido
            if (!d3d12) {
                std::cerr << "Warning: d3d12 es NULL, saltando frame" << std::endl;
                Sleep(100);
                continue;
            }
            
            // Verificar que la ventana aún existe
            if (!IsWindow(hwnd)) {
                std::cerr << "Warning: Ventana cerrada, saliendo del loop" << std::endl;
                running = false;
                break;
            }
            
            d3d12->BeginFrame();
            
            // Obtener command list
            ID3D12GraphicsCommandList* commandList = d3d12->GetCommandQueue()->GetCommandList();
            if (!commandList) {
                std::cerr << "Warning: Command list es NULL, saltando frame" << std::endl;
                d3d12->EndFrame(); // Asegurar que EndFrame se llame
                continue; // Saltar este frame pero continuar el loop
            }
            
            // Verificar dimensiones válidas
            if (appData->width == 0 || appData->height == 0) {
                std::cerr << "Warning: Dimensiones inválidas (" << appData->width << "x" << appData->height << "), saltando frame" << std::endl;
                d3d12->EndFrame();
                continue;
            }
            
            // Establecer viewport y scissor rect PRIMERO (antes de cualquier otro comando)
            D3D12_VIEWPORT viewport = { 0.0f, 0.0f, (float)appData->width, (float)appData->height, 0.0f, 1.0f };
            D3D12_RECT scissorRect = { 0, 0, (LONG)appData->width, (LONG)appData->height };
            commandList->RSSetViewports(1, &viewport);
            commandList->RSSetScissorRects(1, &scissorRect);
            
            // Usar Material System si está disponible, sino usar PSO básico
            bool useMaterial = false;
            if (appData->material && appData->material->IsValid()) {
                try {
                    appData->material->Bind(commandList);
                    useMaterial = true;
                } catch (...) {
                    // Si falla el material, usar PSO básico
                    useMaterial = false;
                }
            }
            
            if (!useMaterial) {
                // Establecer pipeline state básico
                if (pso && pso->GetPSO() && pso->GetRootSignature()) {
                    commandList->SetPipelineState(pso->GetPSO());
                    commandList->SetGraphicsRootSignature(pso->GetRootSignature());
                } else {
                    std::cerr << "Error: PSO básico no válido, saltando frame" << std::endl;
                    d3d12->EndFrame();
                    continue; // Saltar este frame
                }
            }
            
            // Bind constant buffer MVP (siempre necesario, tanto para Material como PSO básico)
            if (pso && pso->HasConstantBuffer() && mvpBuffer) {
                mvpBuffer->Bind(commandList, 0);
            }
            
            // Dibujar el cubo (usar mesh de appData)
            if (appData->mesh) {
                appData->mesh->Draw(commandList);
            }
            
            d3d12->EndFrame();
            
            // Present con manejo de errores
            try {
                d3d12->Present(); // VSync ya está habilitado (syncInterval = 1)
            } catch (...) {
                std::cerr << "Warning: Error en Present(), continuando..." << std::endl;
            }
            
        } catch (const std::exception& ex) {
            std::cerr << "Error en renderizado: " << ex.what() << ", continuando loop..." << std::endl;
            // Asegurar que EndFrame se llame incluso si hay error
            try {
                if (d3d12) {
                    d3d12->EndFrame();
                }
            } catch (...) {
                // Ignorar errores al limpiar
            }
            // Continuar el loop aunque haya errores
        } catch (...) {
            std::cerr << "Error desconocido en renderizado, continuando loop..." << std::endl;
            // Asegurar que EndFrame se llame incluso si hay error
            try {
                if (d3d12) {
                    d3d12->EndFrame();
                }
            } catch (...) {
                // Ignorar errores al limpiar
            }
            // Continuar el loop aunque haya errores
        }
        
        // Control de frame time para mantener 60 FPS suave
        QueryPerformanceCounter(&currentTime);
        double elapsed = (double)(currentTime.QuadPart - lastTime.QuadPart) / frequency.QuadPart;
        
        // Si el frame fue muy rápido, esperar para mantener VSync suave
        if (elapsed < targetFrameTime) {
            double sleepTime = (targetFrameTime - elapsed) * 1000.0;
            if (sleepTime > 0.0 && sleepTime < 16.0) {
                Sleep((DWORD)sleepTime);
            }
        }
        
        lastTime = currentTime;
        loopIterations++;
        
        // Log cada 300 frames (~5 segundos a 60 FPS) para verificar que el loop continúa
        if (frameCount - lastLogFrame >= 300) {
            std::cout << "[Loop activo] Frames renderizados: " << frameCount 
                      << ", Iteraciones: " << loopIterations << std::endl;
            lastLogFrame = frameCount;
        }
    }
    
    std::cout << "Loop terminado. Total frames: " << frameCount 
              << ", Total iteraciones: " << loopIterations << std::endl;

    std::cout << "=== Limpiando recursos ===" << std::endl;

    // Limpiar
    delete mvpBuffer;
    delete cubeMesh;
    if (appData->material) {
        delete appData->material;
    }
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
        CubeConfig config;
    };
    
    AppData* appData = reinterpret_cast<AppData*>(
        GetWindowLongPtr(hwnd, GWLP_USERDATA)
    );
    
    D3D12Core::D3D12Core* d3d12 = appData ? appData->d3d12 : nullptr;

    switch (uMsg) {
    case WM_DESTROY:
        std::cout << "WM_DESTROY recibido en WindowProc" << std::endl;
        PostQuitMessage(0);
        return 0;
    
    case WM_CLOSE:
        std::cout << "WM_CLOSE recibido en WindowProc" << std::endl;
        // No hacer nada aquí, dejar que WM_DESTROY maneje el cierre
        return 0;

    case WM_SIZE:
        if (d3d12 && appData && wParam != SIZE_MINIMIZED) {
            UINT width = LOWORD(lParam);
            UINT height = HIWORD(lParam);
            if (width > 0 && height > 0) {
                appData->width = width;
                appData->height = height;
                d3d12->Resize(width, height);
            }
        }
        return 0;

    case WM_PAINT:
        // El renderizado se hace en el loop principal
        // Validar la región para evitar repintados innecesarios
        ValidateRect(hwnd, nullptr);
        return 0;
    
    case WM_ACTIVATE:
        // Manejar cuando la ventana pierde/gana foco
        // No hacer nada especial, solo continuar renderizando
        return 0;
    }

    return DefWindowProc(hwnd, uMsg, wParam, lParam);
}
