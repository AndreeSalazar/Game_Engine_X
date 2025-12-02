# 🎮 DirectX 12 Research Project - RTX 3060 12GB 🇵🇪

## 👤 Investigador

**Eddi Andreé Salazar Matos** 🇵🇪

---

## 📋 Descripción del Proyecto

Este proyecto es una **investigación completa y detallada** sobre la implementación de DirectX 12 optimizada específicamente para la tarjeta gráfica **NVIDIA GeForce RTX 3060 12GB**. El objetivo principal es demostrar y documentar el aprovechamiento total de las capacidades de DirectX 12 en hardware moderno, utilizando la RTX 3060 12GB como plataforma de prueba.

### 🎯 Objetivos de la Investigación

- ✅ Implementación completa de DirectX 12 desde cero
- ✅ Optimización específica para RTX 3060 12GB
- ✅ Demostración de renderizado 3D con geometría compleja
- ✅ Análisis de rendimiento y uso de memoria VRAM
- ✅ Documentación detallada de cada componente implementado

---

## 🏗️ Estructura del Proyecto

```
DirectX Test/
├── include/                    # Archivos de encabezado (.h)
│   ├── D3D12Core.h            # Clase principal de gestión DirectX 12
│   ├── D3D12Device.h          # Gestión de dispositivo y adaptador
│   ├── D3D12CommandQueue.h    # Cola de comandos y sincronización
│   ├── D3D12SwapChain.h       # Swap chain con triple buffering
│   ├── D3D12DescriptorHeap.h  # Gestión de descriptores
│   ├── D3D12Buffer.h          # Buffers genéricos (vertex/index)
│   ├── D3D12ConstantBuffer.h # Constant buffers para matrices MVP
│   ├── D3D12PipelineState.h   # Pipeline State Objects (PSO)
│   ├── D3D12Mesh.h            # Gestión de mallas 3D
│   └── Shader.h               # Compilador y carga de shaders HLSL
│
├── src/                       # Archivos fuente (.cpp)
│   ├── main.cpp               # Punto de entrada - Renderizado de cubo 3D
│   ├── D3D12Core.cpp         # Implementación del core
│   ├── D3D12Device.cpp       # Implementación del dispositivo
│   ├── D3D12CommandQueue.cpp # Implementación de command queue
│   ├── D3D12SwapChain.cpp    # Implementación del swap chain
│   ├── D3D12DescriptorHeap.cpp
│   ├── D3D12Buffer.cpp
│   ├── D3D12ConstantBuffer.cpp
│   ├── D3D12PipelineState.cpp
│   ├── D3D12Mesh.cpp
│   └── Shader.cpp
│
├── shaders/                   # Shaders HLSL
│   ├── BasicVS.hlsl          # Vertex Shader con transformaciones MVP
│   └── BasicPS.hlsl           # Pixel Shader con gamma correction
│
├── resources/                 # Recursos (texturas, modelos, etc.)
├── CMakeLists.txt            # Configuración CMake
└── README.md                 # Este archivo
```

---

## ⚙️ Requisitos del Sistema

### 📦 Requisitos Mínimos

| Componente | Especificación |
|------------|----------------|
| **Sistema Operativo** | Windows 10 (64-bit) o superior |
| **GPU** | DirectX 12 compatible (Feature Level 12.0+) |
| **RAM** | 8 GB mínimo |
| **Espacio en Disco** | 500 MB para el proyecto |

### 🎯 Requisitos Recomendados (Para RTX 3060 12GB)

| Componente | Especificación |
|------------|----------------|
| **Sistema Operativo** | Windows 11 (64-bit) |
| **GPU** | NVIDIA GeForce RTX 3060 12GB |
| **VRAM** | 12 GB dedicados |
| **RAM** | 16 GB o superior |
| **Driver NVIDIA** | Versión 471.11 o superior |

### 🛠️ Requisitos de Desarrollo

| Herramienta | Versión Mínima | Descripción |
|-------------|----------------|-------------|
| **Visual Studio** | 2019 o superior | IDE y compilador MSVC |
| **CMake** | 3.15 o superior | Sistema de construcción |
| **Windows SDK** | 10.0.19041.0 o superior | SDK de Windows |
| **DirectX 12** | Incluido en Windows SDK | API de gráficos |
| **C++ Standard** | C++20 | Estándar de lenguaje |

### 📥 Instalación de Requisitos

1. **Visual Studio 2022** (Recomendado)
   - Descargar desde: https://visualstudio.microsoft.com/
   - Instalar workload "Desktop development with C++"
   - Incluir "Windows 10/11 SDK"

2. **CMake**
   - Descargar desde: https://cmake.org/download/
   - Agregar al PATH del sistema

3. **NVIDIA Drivers**
   - Descargar desde: https://www.nvidia.com/drivers
   - Instalar la versión más reciente para RTX 3060

---

## 🚀 Compilación y Ejecución

### Método 1: Visual Studio con CMake (Recomendado)

1. **Abrir Visual Studio**
   ```
   File > Open > CMake...
   ```

2. **Seleccionar CMakeLists.txt**
   - Navegar al directorio del proyecto
   - Seleccionar `CMakeLists.txt`

3. **Configurar el proyecto**
   - Visual Studio detectará automáticamente CMake
   - Seleccionar configuración: **Debug** o **Release**
   - Seleccionar plataforma: **x64**

4. **Compilar**
   - Presionar **F7** o
   - `Build > Build All`

5. **Ejecutar**
   - Presionar **F5** o
   - `Debug > Start Debugging`

### Método 2: CMake desde Terminal

1. **Abrir terminal en el directorio del proyecto**

2. **Crear directorio de build**
   ```bash
   mkdir build
   cd build
   ```

3. **Generar archivos del proyecto**
   ```bash
   cmake .. -G "Visual Studio 17 2022" -A x64
   ```

4. **Abrir solución en Visual Studio**
   ```bash
   start DirectX12Test.sln
   ```

5. **Compilar y ejecutar desde Visual Studio**
   - Presionar **F7** para compilar
   - Presionar **F5** para ejecutar

### Método 3: Compilación desde Línea de Comandos

```bash
cd build
cmake --build . --config Release
.\Release\DirectX12Test.exe
```

---

## ✨ Características Implementadas

### 🎨 Renderizado 3D

- ✅ **Cubo 3D Rotatorio**: Renderizado de geometría 3D con rotación multi-eje
- ✅ **Sistema de Colores**: Colores vibrantes por vértice con gamma correction
- ✅ **Transformaciones MVP**: Model-View-Projection matrices correctamente implementadas
- ✅ **Cámara Perspectiva**: Cámara con proyección en perspectiva

### 🔧 Componentes DirectX 12

#### Core Components
- ✅ **D3D12Core**: Clase principal que gestiona todo el sistema DirectX 12
- ✅ **D3D12Device**: Gestión de dispositivo y selección automática de adaptador
- ✅ **D3D12CommandQueue**: Cola de comandos con sincronización mediante Fences
- ✅ **D3D12SwapChain**: Swap chain con triple buffering (3 back buffers)

#### Rendering Pipeline
- ✅ **D3D12PipelineState**: Pipeline State Objects (PSO) con root signatures
- ✅ **D3D12Mesh**: Gestión de mallas 3D con vertex e index buffers
- ✅ **D3D12ConstantBuffer**: Constant buffers para matrices MVP
- ✅ **D3D12Buffer**: Buffers genéricos con upload heaps

#### Shaders
- ✅ **Vertex Shader**: Transformaciones MVP con matrices transpuestas
- ✅ **Pixel Shader**: Renderizado de colores con gamma correction
- ✅ **Shader Compiler**: Compilación en tiempo de ejecución con D3DCompile

#### Descriptores
- ✅ **D3D12DescriptorHeap**: Gestión de descriptor heaps para RTVs
- ✅ **Render Target Views**: Configuración correcta de RTVs para cada back buffer

### 🎯 Optimizaciones Específicas para RTX 3060 12GB

| Optimización | Descripción | Beneficio |
|--------------|-------------|-----------|
| **Triple Buffering** | 3 back buffers en lugar de 2 | Reduce stuttering, mejor rendimiento |
| **Feature Level 12.0** | Soporte completo de DirectX 12 | Acceso a todas las características modernas |
| **Flip Model Swap Chain** | DXGI_SWAP_EFFECT_FLIP_DISCARD | Menor latencia, mejor rendimiento |
| **Selección de Adaptador** | Automática por VRAM dedicada | Aprovecha los 12GB de VRAM |
| **Command List Reutilización** | Reset de command lists por frame | Menor overhead de memoria |
| **Fence Synchronization** | Sincronización GPU-CPU eficiente | Evita bloqueos innecesarios |

### 📊 Especificaciones Técnicas

- **Back Buffers**: 3 (Triple Buffering)
- **Format**: DXGI_FORMAT_R8G8B8A8_UNORM
- **Max Frames In Flight**: 3
- **Viewport**: 1280x720 (configurable)
- **FPS Target**: 60 FPS con control de frame time

---

## 🔬 Detalles de la Investigación

### Arquitectura del Sistema

El proyecto implementa una arquitectura modular y extensible:

```
┌─────────────────────────────────────┐
│         D3D12Core (Manager)         │
├─────────────────────────────────────┤
│  ┌──────────┐  ┌──────────┐        │
│  │  Device  │  │ Command  │        │
│  │          │  │  Queue   │        │
│  └──────────┘  └──────────┘        │
│  ┌──────────┐  ┌──────────┐        │
│  │  Swap    │  │Pipeline  │        │
│  │  Chain   │  │  State   │        │
│  └──────────┘  └──────────┘        │
└─────────────────────────────────────┘
```

### Flujo de Renderizado

1. **Inicialización**
   - Crear dispositivo DirectX 12
   - Crear command queue y command list
   - Crear swap chain con 3 back buffers
   - Crear descriptor heap para RTVs
   - Compilar shaders HLSL

2. **Por Frame**
   - Reset command list
   - Transición de back buffer a RENDER_TARGET
   - Clear render target
   - Establecer pipeline state
   - Bind constant buffer (matrices MVP)
   - Draw mesh (cubo 3D)
   - Transición de back buffer a PRESENT
   - Ejecutar command list
   - Present swap chain

3. **Sincronización**
   - Fence para sincronizar GPU-CPU
   - Frame index para triple buffering

### Gestión de Memoria

- **Upload Heaps**: Para subir datos de CPU a GPU
- **Default Heaps**: Para recursos GPU-only
- **Constant Buffers**: Upload heap mapeado para actualización rápida
- **Vertex/Index Buffers**: Upload temporal + copia a default heap

---

## 📚 Documentación Técnica

### Clases Principales

#### `D3D12Core`
Clase principal que gestiona todo el sistema DirectX 12.

**Métodos principales:**
- `Initialize()`: Inicializa todos los componentes
- `BeginFrame()`: Prepara el frame para renderizado
- `EndFrame()`: Finaliza el frame y ejecuta comandos
- `Present()`: Presenta el frame al swap chain

#### `D3D12Device`
Gestiona el dispositivo DirectX 12 y la selección del adaptador.

**Características:**
- Selección automática del adaptador con más VRAM
- Creación de factory DXGI
- Información del adaptador (nombre, VRAM)

#### `D3D12CommandQueue`
Gestiona la cola de comandos y la sincronización.

**Características:**
- Command allocators por frame (triple buffering)
- Fence para sincronización GPU-CPU
- Reset y ejecución de command lists

#### `D3D12SwapChain`
Gestiona el swap chain con triple buffering.

**Características:**
- 3 back buffers
- Flip model para mejor rendimiento
- Resize automático de back buffers

#### `D3D12PipelineState`
Gestiona Pipeline State Objects y Root Signatures.

**Características:**
- Root signature con constant buffer
- Input layout para vertex data
- Rasterizer state configurado

#### `D3D12Mesh`
Gestiona mallas 3D con vertex e index buffers.

**Características:**
- Vertex buffer con posición y color
- Index buffer para renderizado indexado
- Upload automático de datos a GPU

#### `D3D12ConstantBuffer`
Gestiona constant buffers para datos de CPU a GPU.

**Características:**
- Alineamiento a 256 bytes (requerimiento D3D12)
- Upload heap mapeado para actualización rápida
- Binding a root signature

### Shaders

#### Vertex Shader (`BasicVS.hlsl`)
- Recibe posición y color por vértice
- Aplica transformaciones MVP (Model-View-Projection)
- Multiplicación correcta de matrices (row-major)

#### Pixel Shader (`BasicPS.hlsl`)
- Recibe color interpolado del vertex shader
- Aplica gamma correction para colores más vivos
- Retorna color final con alpha

---

## 🐛 Solución de Problemas

### Error: "CMake no encontrado"
**Solución**: Agregar CMake al PATH del sistema o usar la ruta completa.

### Error: "Visual Studio no encontrado"
**Solución**: Asegurarse de tener Visual Studio 2019+ instalado con C++ workload.

### Error: "Shaders no encontrados"
**Solución**: Los shaders se copian automáticamente. Verificar que `shaders/` existe en el directorio de salida.

### Ventana en blanco
**Solución**: 
- Verificar que la GPU soporta DirectX 12
- Actualizar drivers de NVIDIA
- Revisar la consola para mensajes de error

### Bajo rendimiento
**Solución**:
- Compilar en modo Release (no Debug)
- Verificar que se está usando la RTX 3060 (no GPU integrada)
- Cerrar otras aplicaciones que usen GPU

---

## 🔮 Próximas Mejoras

### Funcionalidades Planificadas

- [ ] **Texturas**: Carga y renderizado de texturas
- [ ] **Iluminación**: Sistema de iluminación Phong/Blinn-Phong
- [ ] **Modelos 3D**: Carga de modelos desde archivos (OBJ, FBX)
- [ ] **Depth Buffer**: Z-buffer para renderizado correcto
- [ ] **Compute Shaders**: Procesamiento paralelo en GPU
- [ ] **Ray Tracing (DXR)**: Aprovechar RT Cores de RTX 3060
- [ ] **Mesh Shaders**: Shaders de malla para geometría compleja
- [ ] **Multi-threading**: Renderizado multi-threaded
- [ ] **Profiling**: Herramientas de análisis de rendimiento

---

## 📝 Notas Técnicas

### Namespace
El proyecto usa el namespace `D3D12Core` para organizar todo el código.

### Compilación de Shaders
Los shaders se compilan en tiempo de ejecución usando `D3DCompile` de `d3dcompiler.lib`.

### Modo Debug
En modo Debug, se habilita automáticamente la capa de debug de DirectX 12 para detectar errores.

### C++20
El proyecto utiliza C++20 para características modernas del lenguaje.

---

## 📄 Licencia

Este es un proyecto de **investigación y educación**. Siéntete libre de usarlo, estudiarlo y modificarlo según tus necesidades.

---

## 👨‍💻 Autor

**Eddi Andreé Salazar Matos** 🇵🇪

*Investigador en Gráficos por Computadora y DirectX 12*

---

## 🙏 Agradecimientos

- Microsoft por DirectX 12 y la documentación
- NVIDIA por la RTX 3060 12GB y los drivers
- Comunidad de desarrolladores de gráficos

---

**Última actualización**: 2024

**Versión del proyecto**: 1.0.0

**Estado**: ✅ Funcional y en desarrollo activo
