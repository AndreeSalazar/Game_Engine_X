# 🎨 Guía: UIs Integradas - Estilo Unreal Engine 5

## ✅ Integración Completa de UIs

### 🎯 Ventanas Disponibles

#### 1. **MainWindow** (Editor Principal)
**Ubicación**: `Engine/Editor/DirectX12Editor/MainWindow.xaml`

**Funcionalidades:**
- ✅ **Details Panel** - Control de transformación del cubo
- ✅ **Camera Controls** - Posición y FOV
- ✅ **Auto Rotate** - Control de rotación automática
- ✅ **Statistics Panel** - Valores actuales en tiempo real
- ✅ **Botón Material Editor** - Abre Material Editor desde aquí

**Controles:**
- Rotation Speed (0 - 0.1)
- Scale (0.1 - 2.0)
- Rotation X Multiplier (0 - 2.0)
- Camera Position (X, Y, Z)
- FOV (0.1 - 2.0 radians)
- Auto Rotate (Checkbox)

---

#### 2. **MaterialEditor** (Editor de Materiales)
**Ubicación**: `Engine/Editor/DirectX12Editor/MaterialEditor.xaml`

**Funcionalidades:**
- ✅ **Material Properties** - Base Color, Metallic, Roughness, Emissive
- ✅ **Material Graph** - Área para futuro editor de nodos
- ✅ **Preview & Info** - Preview del material y estadísticas
- ✅ **Auto-save** - Guarda automáticamente cada 2 segundos
- ✅ **Botón Main Editor** - Vuelve al editor principal

**Controles:**
- Base Color (R, G, B) - Sliders 0-1
- Metallic - Slider 0-1
- Roughness - Slider 0-1
- Emissive Color (R, G, B) - Sliders 0-10

---

## 🔗 Navegación entre Ventanas

### Desde MainWindow:
1. Click en **"🎨 Open Material Editor"** (botón verde)
2. Se abre Material Editor
3. Puedes tener ambas ventanas abiertas simultáneamente

### Desde MaterialEditor:
1. Click en **"📊 Open Main Editor"** (botón azul)
2. Trae MainWindow al frente
3. O crea nueva instancia si no existe

---

## 🚀 Cómo Usar el Sistema Completo

### Paso 1: Ejecutar DirectX12Test.exe
```
Engine/Binaries/Win64/DirectX12Test.exe
```
O desde Visual Studio: F5

### Paso 2: Ejecutar Editor Principal
```
Engine/Editor/DirectX12Editor/bin/Debug/net8.0-windows/DirectX12Editor.exe
```
O desde Visual Studio: Click derecho en proyecto C# → Debug → Start New Instance

### Paso 3: Abrir Material Editor
- Click en **"🎨 Open Material Editor"** en MainWindow
- O ejecutar MaterialEditor directamente

### Paso 4: Configurar
- **En MainWindow**: Ajusta rotación, escala, cámara
- **En MaterialEditor**: Ajusta Base Color, Metallic, Roughness
- **Cambios en tiempo real**: Se aplican automáticamente a DirectX12Test.exe

---

## 📋 Flujo de Trabajo Recomendado

### Workflow 1: Configuración Básica
1. Abrir MainWindow
2. Ajustar controles de transformación
3. Ver cambios en DirectX12Test.exe en tiempo real

### Workflow 2: Edición de Materiales
1. Abrir Material Editor desde MainWindow
2. Ajustar Base Color, Metallic, Roughness
3. Ver cambios aplicados automáticamente cada 2 segundos
4. Guardar material para uso futuro

### Workflow 3: Trabajo Completo
1. Abrir ambas ventanas (MainWindow + MaterialEditor)
2. Configurar transformación en MainWindow
3. Configurar material en MaterialEditor
4. Ver resultado combinado en DirectX12Test.exe

---

## 🎨 Características Estilo Unreal Engine 5

### Tema Visual:
- ✅ Fondo oscuro (#FF1E1E1E)
- ✅ Bordes azules (#FF0078D4)
- ✅ Texto claro (#FFE8E8E8)
- ✅ Sliders azules profesionales
- ✅ Botones con hover effects
- ✅ Tooltips informativos

### Interactividad:
- ✅ Auto-save en Material Editor
- ✅ Actualización en tiempo real
- ✅ Navegación fluida entre ventanas
- ✅ Estados visuales (conectado/desconectado)
- ✅ Preview de colores

---

## 🔧 Archivos de Configuración

### MainWindow guarda en:
- `Engine/Binaries/Win64/config.json`
- `x64/Debug/config.json`
- `config.json` (fallback)

### MaterialEditor guarda en:
- `Engine/Binaries/Win64/current_material.json` (aplicación en tiempo real)
- `Engine/Content/Materials/[Nombre].json` (guardado permanente)

---

## 📊 Estado de Conexión

### MainWindow muestra:
- ✅ **Verde**: DirectX12Test.exe ejecutándose
- ⚠️ **Naranja**: DirectX12Test.exe no encontrado

### MaterialEditor muestra:
- ✅ **Verde**: Material aplicado correctamente
- ❌ **Rojo**: Error al aplicar material

---

## 🎯 Próximas Mejoras Sugeridas

### UI:
- [ ] Preview 3D del cubo en Material Editor
- [ ] Timeline para animaciones
- [ ] Asset Browser integrado
- [ ] Shader Editor visual

### Funcionalidad:
- [ ] Aplicar BaseColor al cubo visualmente
- [ ] Sistema de iluminación básico
- [ ] Carga de texturas
- [ ] Material Instances

---

**Autor**: Eddi Andreé Salazar Matos 🇵🇪  
**Proyecto**: DirectX 12 Research - RTX 3060 12GB

