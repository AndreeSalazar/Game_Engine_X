# 🎨 Guía Completa: Editor C# UI Estilo Unreal Engine 5

## 📋 Resumen

Esta guía te muestra cómo crear y usar un editor visual profesional en C# WPF para controlar tu proyecto DirectX 12 en tiempo real, similar a las herramientas de Unreal Engine 5.

---

## 🚀 Paso 1: Modificaciones al Proyecto C++

### ✅ Cambios Realizados

El proyecto C++ (`DirectX12Test.exe`) ha sido modificado para:

1. **Leer configuración desde `config.json`** cada frame
2. **Aplicar cambios en tiempo real** sin reiniciar
3. **Soporte para múltiples propiedades** configurables

### Archivos Modificados

- `src/main.cpp`:
  - Agregada función `LoadConfig()` para leer JSON
  - Estructura `CubeConfig` para almacenar configuración
  - `AppData` actualizado con `CubeConfig`
  - Loop principal lee `config.json` cada frame

### Propiedades Configurables

- `rotationSpeed`: Velocidad de rotación
- `scale`: Escala del cubo
- `rotationXMultiplier`: Multiplicador de rotación X
- `cameraX/Y/Z`: Posición de la cámara
- `fov`: Campo de visión
- `autoRotate`: Rotación automática on/off

---

## 🎨 Paso 2: Crear Proyecto C# WPF

### Estructura del Proyecto

```
DirectX12Editor/
├── DirectX12Editor.csproj      # Archivo de proyecto
├── MainWindow.xaml              # UI principal
├── MainWindow.xaml.cs           # Lógica de la UI
├── App.xaml                     # Aplicación WPF
├── App.xaml.cs                  # Código de aplicación
└── README.md                     # Documentación
```

### Crear el Proyecto en Visual Studio

1. **Abrir Visual Studio 2022**
2. **File > New > Project**
3. **Seleccionar**: "WPF App (.NET)" o "WPF Application"
4. **Nombre**: `DirectX12Editor`
5. **Framework**: .NET 8.0 o superior
6. **Ubicación**: Junto a tu proyecto DirectX 12

### Agregar Paquetes NuGet

1. **Click derecho en el proyecto** > **Manage NuGet Packages**
2. **Buscar e instalar**:
   - `Newtonsoft.Json` (versión 13.0.3 o superior)

O usar Package Manager Console:
```powershell
Install-Package Newtonsoft.Json
```

---

## 📝 Paso 3: Copiar Archivos del Editor

### Opción A: Copiar Archivos Manualmente

1. Copia `MainWindow.xaml` al proyecto
2. Copia `MainWindow.xaml.cs` al proyecto
3. Reemplaza `App.xaml` y `App.xaml.cs` si es necesario

### Opción B: Crear desde Cero

Sigue la estructura de los archivos proporcionados y copia el contenido.

---

## 🔧 Paso 4: Configurar el Proyecto

### Asegurar que el Archivo de Configuración se Cree Correctamente

El editor busca `config.json` en:
1. `x64/Debug/config.json` (junto a DirectX12Test.exe)
2. Directorio actual del editor

**Importante**: Asegúrate de que DirectX12Test.exe tenga permisos de escritura en su directorio.

---

## ▶️ Paso 5: Compilar y Ejecutar

### Compilar el Editor C#

1. **Abrir** `DirectX12Editor.csproj` en Visual Studio
2. **Restaurar paquetes NuGet** (automático al abrir)
3. **Compilar** (F7 o Build > Build Solution)
4. **Verificar** que no hay errores

### Ejecutar el Sistema Completo

1. **Primero**: Ejecuta `DirectX12Test.exe`
   - Ubicación: `x64/Debug/DirectX12Test.exe`
   - O desde Visual Studio (F5)

2. **Segundo**: Ejecuta `DirectX12Editor.exe`
   - Ubicación: `DirectX12Editor/bin/Debug/net8.0-windows/DirectX12Editor.exe`
   - O desde Visual Studio (F5)

3. **Resultado**: 
   - El editor se abre con UI estilo Unreal Engine 5
   - Los cambios en los controles se aplican automáticamente al cubo
   - El archivo `config.json` se crea/actualiza automáticamente

---

## 🎮 Uso del Editor

### Panel Izquierdo: Details Panel

#### Transformación
- **Rotation Speed Slider**: Ajusta velocidad de rotación (0.0 - 0.1)
- **Scale Slider**: Cambia tamaño del cubo (0.1 - 2.0)
- **Rotation X Multiplier**: Controla rotación en eje X (0.0 - 2.0)

#### Cámara
- **Position X/Y/Z**: Mueve la cámara en 3D
- **FOV Slider**: Cambia campo de visión

#### Controles
- **Auto Rotate Checkbox**: Activa/desactiva rotación automática
- **Reset to Default**: Restaura valores iniciales
- **Save Config**: Guarda configuración manualmente
- **Load Config**: Recarga configuración desde archivo

### Panel Central: Viewport Info

- Muestra estado de la conexión
- Información de rendimiento
- Ruta del archivo de configuración

### Panel Derecho: Statistics

- Valores actuales en tiempo real
- Instrucciones de uso
- Información del sistema

---

## 🔍 Solución de Problemas

### El cubo no responde a los cambios

1. **Verificar** que DirectX12Test.exe esté ejecutándose
2. **Verificar** que `config.json` se esté creando en el directorio correcto
3. **Revisar** la consola de DirectX12Test.exe para errores
4. **Asegurar** que el archivo `config.json` tenga formato JSON válido

### El editor no encuentra config.json

1. **Verificar** la ruta mostrada en el editor
2. **Crear** el archivo manualmente si es necesario
3. **Asegurar** permisos de escritura en el directorio

### Errores de compilación C#

1. **Verificar** que .NET 8.0 esté instalado
2. **Restaurar** paquetes NuGet
3. **Verificar** que todos los archivos estén en el proyecto

### Errores de compilación C++

1. **Verificar** que `#include <fstream>` esté presente
2. **Verificar** que la función `LoadConfig()` esté implementada
3. **Revisar** que `CubeConfig` esté definido antes de `AppData`

---

## 🎯 Características Avanzadas

### Personalizar la UI

Puedes modificar `MainWindow.xaml` para:
- Cambiar colores y temas
- Agregar más controles
- Modificar el layout
- Agregar nuevas propiedades

### Agregar Nuevas Propiedades

1. **En C++** (`main.cpp`):
   - Agregar campo a `CubeConfig`
   - Leer desde JSON en `LoadConfig()`
   - Usar en el loop de renderizado

2. **En C#** (`MainWindow.xaml`):
   - Agregar control (Slider, TextBox, etc.)
   - Agregar event handler en `MainWindow.xaml.cs`
   - Guardar en `SaveConfig()`

### Ejemplo: Agregar Color del Cubo

**En C++:**
```cpp
struct CubeConfig {
    // ... campos existentes
    float cubeColorR = 1.0f;
    float cubeColorG = 0.0f;
    float cubeColorB = 0.0f;
};
```

**En C#:**
```csharp
// En SaveConfig()
cubeColorR = ParseFloat(CubeColorRTextBox.Text, 1.0f),
cubeColorG = ParseFloat(CubeColorGTextBox.Text, 0.0f),
cubeColorB = ParseFloat(CubeColorBTextBox.Text, 0.0f),
```

---

## 📚 Recursos Adicionales

### Documentación WPF
- [Microsoft WPF Documentation](https://docs.microsoft.com/en-us/dotnet/desktop/wpf/)
- [WPF Tutorials](https://www.wpf-tutorial.com/)

### JSON en C#
- [Newtonsoft.Json Documentation](https://www.newtonsoft.com/json)

### DirectX 12
- [DirectX 12 Documentation](https://docs.microsoft.com/en-us/windows/win32/direct3d12/direct3d-12-graphics)

---

## ✅ Checklist de Implementación

- [x] Modificar `main.cpp` para leer `config.json`
- [x] Crear proyecto C# WPF
- [x] Implementar UI estilo Unreal Engine 5
- [x] Implementar sistema de guardado/carga de configuración
- [x] Probar integración entre C# y C++
- [x] Documentar uso y solución de problemas

---

## 🎉 Resultado Final

Al completar esta guía tendrás:

1. ✅ **Editor visual profesional** estilo Unreal Engine 5
2. ✅ **Control en tiempo real** del cubo 3D
3. ✅ **Interfaz moderna** con Fluent Design
4. ✅ **Sistema de configuración persistente**
5. ✅ **Base sólida** para expandir con más herramientas

---

**Autor**: Eddi Andreé Salazar Matos 🇵🇪  
**Proyecto**: DirectX 12 Research - RTX 3060 12GB  
**Fecha**: 2024

