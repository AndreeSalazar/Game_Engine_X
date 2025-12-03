# DirectX 12 Editor - UI Profesional Estilo Unreal Engine 5

Editor visual profesional para controlar DirectX12Test.exe en tiempo real.

## 🚀 Características

- ✅ **Details Panel** estilo Unreal Engine 5
- ✅ **Control en tiempo real** del cubo 3D
- ✅ **Interfaz oscura moderna** con Fluent Design
- ✅ **Configuración persistente** mediante JSON
- ✅ **Actualización automática** cada segundo

## 📋 Requisitos

- .NET 8.0 o superior
- Visual Studio 2022
- DirectX12Test.exe ejecutándose

## 🛠️ Compilación

1. Abre `DirectX12Editor.csproj` en Visual Studio
2. Restaura paquetes NuGet (automático)
3. Compila (F7)
4. Ejecuta (F5)

## 📖 Uso

1. **Ejecuta DirectX12Test.exe** primero
2. **Ejecuta DirectX12Editor.exe**
3. **Ajusta los controles** - Los cambios se aplican automáticamente
4. **Guarda la configuración** para reutilizarla

## 🎨 Controles Disponibles

### Transformación
- **Rotation Speed**: Velocidad de rotación (0.0 - 0.1)
- **Scale**: Escala del cubo (0.1 - 2.0)
- **Rotation X Multiplier**: Multiplicador de rotación en X (0.0 - 2.0)

### Cámara
- **Position X/Y/Z**: Posición de la cámara
- **FOV**: Campo de visión en radianes

### Controles
- **Auto Rotate**: Activar/desactivar rotación automática
- **Reset to Default**: Restaurar valores por defecto
- **Save Config**: Guardar configuración manualmente
- **Load Config**: Cargar configuración guardada

## 📁 Archivo de Configuración

El editor guarda la configuración en `config.json` en el directorio donde se ejecuta DirectX12Test.exe.

Formato:
```json
{
  "rotationSpeed": 0.015,
  "scale": 0.6,
  "rotationXMultiplier": 0.7,
  "cameraX": 0.0,
  "cameraY": 1.0,
  "cameraZ": -4.0,
  "fov": 0.785398,
  "autoRotate": true
}
```

## 🔧 Integración con DirectX12Test.exe

El editor funciona mediante archivo JSON compartido:
1. Editor escribe `config.json`
2. DirectX12Test.exe lee `config.json` cada frame
3. Cambios se aplican en tiempo real

## 🎯 Próximas Mejoras

- [ ] Preview del cubo en tiempo real
- [ ] Editor de shaders HLSL
- [ ] Performance profiler integrado
- [ ] Asset browser
- [ ] Scripting system con Lua

---

**Autor**: Eddi Andreé Salazar Matos 🇵🇪  
**Proyecto**: DirectX 12 Research - RTX 3060 12GB

