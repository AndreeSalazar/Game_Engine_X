# 🚀 Guía Completa: Ejecutar C# + C++ en Tiempo Real

## 📋 Resumen del Sistema

Tu proyecto tiene **DOS aplicaciones** que trabajan juntas:

1. **DirectX12Test.exe** (C++) - Renderiza el cubo 3D
2. **DirectX12Editor.exe** (C#) - Editor con controles para ajustar parámetros

**Comunicación**: El editor C# escribe `config.json` → La app C++ lee `config.json` cada frame → **Cambios en tiempo real** ✨

---

## 🎯 Método Recomendado: Ejecutar Ambos Simultáneamente

### Opción 1: Visual Studio + Explorador de Archivos (Más Fácil)

#### Paso 1: Compilar Ambos Proyectos
1. Abre **Visual Studio**
2. Abre la solución: `DirectX12Test.sln`
3. Presiona **F7** (Build Solution) para compilar ambos proyectos

#### Paso 2: Ejecutar DirectX12Test.exe (C++)
1. En **Solution Explorer**, click derecho en `DirectX12Test`
2. Selecciona **"Set as StartUp Project"**
3. Presiona **F5** (Start Debugging)
   - ✅ Deberías ver una ventana con el cubo 3D rotando

#### Paso 3: Ejecutar DirectX12Editor.exe (C#)
1. Abre **File Explorer** (Explorador de archivos)
2. Navega a:
   ```
   DirectX Test\Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\
   ```
3. Doble click en **`DirectX12Editor.exe`**
   - ✅ Deberías ver la ventana del editor con controles

#### Paso 4: ¡Probar en Tiempo Real!
1. En el editor C#, ajusta los **sliders** (Rotation Speed, Scale, etc.)
2. Los cambios se guardan automáticamente en `config.json`
3. El cubo 3D debería cambiar **inmediatamente** ✨

---

### Opción 2: Dos Instancias de Visual Studio

#### Instancia 1: DirectX12Test (C++)
1. Abre Visual Studio
2. Abre `DirectX12Test.sln`
3. Click derecho en `DirectX12Test` → **"Set as StartUp Project"**
4. Presiona **F5**

#### Instancia 2: DirectX12Editor (C#)
1. Abre **otra ventana** de Visual Studio
2. Abre el mismo proyecto (`DirectX12Test.sln`)
3. Click derecho en `DirectX12Editor` → **"Set as StartUp Project"**
4. Presiona **F5**

---

### Opción 3: Desde Línea de Comandos (PowerShell)

Crea un script para ejecutar ambos automáticamente:

```powershell
# Ejecutar ambos proyectos
# Guarda esto como "EjecutarTodo.ps1" en la raíz del proyecto

# Compilar C++
Write-Host "Compilando DirectX12Test..." -ForegroundColor Cyan
& msbuild DirectX12Test.vcxproj /p:Configuration=Debug /p:Platform=x64

# Compilar C#
Write-Host "Compilando DirectX12Editor..." -ForegroundColor Cyan
& dotnet build Engine\Editor\DirectX12Editor\DirectX12Editor.csproj

# Ejecutar C++ en background
Write-Host "Ejecutando DirectX12Test..." -ForegroundColor Green
Start-Process -FilePath "x64\Debug\DirectX12Test.exe" -WindowStyle Normal

# Esperar un poco
Start-Sleep -Seconds 2

# Ejecutar C#
Write-Host "Ejecutando DirectX12Editor..." -ForegroundColor Green
Start-Process -FilePath "Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe" -WindowStyle Normal

Write-Host "¡Ambos proyectos ejecutándose!" -ForegroundColor Yellow
```

**Uso:**
```powershell
.\EjecutarTodo.ps1
```

---

## 📁 Ubicaciones de Archivos Importantes

### Ejecutables:
- **C++**: `x64\Debug\DirectX12Test.exe`
- **C#**: `Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe`

### Archivo de Configuración (config.json):
El editor C# busca/escribe en estas ubicaciones (en orden):
1. `Engine\Binaries\Win64\config.json` ⭐ (Prioridad)
2. `x64\Debug\config.json`
3. `config.json` (raíz del proyecto)

La aplicación C++ lee desde las mismas ubicaciones.

---

## 🔧 Verificar que Funciona

### Checklist Pre-Ejecución:
- [ ] Ambos proyectos compilan sin errores (F7)
- [ ] `DirectX12Test.exe` existe en `x64\Debug\`
- [ ] `DirectX12Editor.exe` existe en `Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\`
- [ ] Los shaders existen: `Engine\Rendering\Shaders\BasicVS.hlsl` y `BasicPS.hlsl`

### Durante la Ejecución:
1. **DirectX12Test.exe** debe mostrar:
   - ✅ Ventana con título "DirectX 12 - RTX 3060 12GB"
   - ✅ Cubo 3D rotando con colores vibrantes
   - ✅ Consola con mensajes de debug

2. **DirectX12Editor.exe** debe mostrar:
   - ✅ Ventana con título "DirectX 12 Editor - RTX 3060 12GB"
   - ✅ Panel izquierdo: "Details Panel" con sliders
   - ✅ Panel central: "DirectX 12 Viewport" con estado
   - ✅ Panel derecho: "Statistics" con valores actuales
   - ✅ Botón verde: "🎨 Open Material Editor"

### Prueba de Tiempo Real:
1. Mueve el slider **"Rotation Speed"** en el editor
2. El cubo debería rotar **más rápido o más lento** inmediatamente
3. Mueve el slider **"Scale"**
4. El cubo debería **agrandarse o achicarse** en tiempo real

---

## 🐛 Solución de Problemas

### Problema: El cubo no cambia cuando ajusto los sliders

**Causas posibles:**
1. El archivo `config.json` no se está escribiendo en la ubicación correcta
2. La aplicación C++ no está leyendo el archivo correcto
3. Los directorios no existen

**Solución:**
1. Verifica que existe el directorio: `Engine\Binaries\Win64\`
2. Si no existe, créalo manualmente
3. Verifica que el editor está escribiendo ahí:
   - Abre el editor C#
   - Ajusta un slider
   - Verifica que se creó `Engine\Binaries\Win64\config.json`
4. Verifica que la aplicación C++ está leyendo desde ahí:
   - Revisa la consola de `DirectX12Test.exe`
   - Debería mostrar mensajes sobre cargar configuración

### Problema: No veo la ventana del editor C#

**Solución:**
1. Compila el proyecto C#: `Build → Build Solution` (F7)
2. Verifica que el .exe existe en la ruta correcta
3. Ejecuta manualmente desde File Explorer
4. Revisa la ventana **Output** en Visual Studio para errores

### Problema: Error "config.json no encontrado"

**Solución:**
1. El editor C# creará el archivo automáticamente al guardar
2. Si no se crea, verifica permisos de escritura
3. Crea manualmente el directorio: `Engine\Binaries\Win64\`
4. El editor debería crear el archivo al mover cualquier slider

### Problema: Los cambios no se aplican en tiempo real

**Verifica:**
1. ¿Ambas aplicaciones están ejecutándose?
2. ¿El archivo `config.json` se está actualizando? (abre el archivo y verifica)
3. ¿La aplicación C++ está leyendo cada frame? (línea 455 en main.cpp: `LoadConfig(appData->config);`)

---

## 📊 Flujo de Datos en Tiempo Real

```
┌─────────────────────┐
│ DirectX12Editor.exe │
│    (C# WPF App)     │
│                     │
│  Sliders/Controles  │
└──────────┬──────────┘
           │
           │ Escribe JSON
           ▼
┌─────────────────────┐
│  config.json         │
│  (Engine/Binaries/   │
│   Win64/config.json) │
└──────────┬──────────┘
           │
           │ Lee cada frame
           ▼
┌─────────────────────┐
│ DirectX12Test.exe   │
│    (C++ DirectX12)   │
│                     │
│  Renderiza Cubo 3D  │
└─────────────────────┘
```

**Frecuencia de actualización:**
- **Editor C#**: Guarda cada vez que cambias un slider (inmediato)
- **App C++**: Lee `config.json` cada frame (~60 veces por segundo)

---

## 💡 Tips y Mejores Prácticas

### Para Desarrollo:
1. **Mantén ambas ventanas visibles** lado a lado
2. **Usa dos monitores** si es posible (uno para el editor, otro para el cubo)
3. **Ajusta primero el editor**, luego observa los cambios en el cubo

### Para Debugging:
1. **Consola de DirectX12Test**: Muestra mensajes de debug y errores
2. **Output de Visual Studio**: Muestra errores de compilación
3. **Verifica config.json**: Abre el archivo para ver los valores actuales

### Optimización:
- El editor tiene un **timer de 1 segundo** para actualizar estadísticas
- La app C++ lee el archivo **cada frame** (muy eficiente para archivos pequeños)
- Los cambios son **inmediatos** sin necesidad de reiniciar

---

## 🎮 Comandos Rápidos

### Compilar Todo:
```powershell
# En Visual Studio
F7 (Build Solution)
```

### Ejecutar C++:
```powershell
# En Visual Studio
F5 (con DirectX12Test como StartUp Project)
```

### Ejecutar C#:
```powershell
# Desde File Explorer
.\Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe
```

### Verificar config.json:
```powershell
# Ver contenido
Get-Content Engine\Binaries\Win64\config.json
```

---

## ✅ Checklist Final

Antes de empezar a trabajar:

- [ ] ✅ Ambos proyectos compilan sin errores
- [ ] ✅ DirectX12Test.exe ejecuta y muestra el cubo
- [ ] ✅ DirectX12Editor.exe ejecuta y muestra la UI
- [ ] ✅ El archivo config.json se crea/actualiza al mover sliders
- [ ] ✅ Los cambios se reflejan en tiempo real en el cubo
- [ ] ✅ Puedes ajustar Rotation Speed, Scale, Camera, FOV
- [ ] ✅ El Material Editor se abre desde el botón verde

---

## 🚀 ¡Listo para Trabajar!

Ahora puedes:
- ✅ Ajustar parámetros del cubo en tiempo real
- ✅ Ver cambios inmediatamente sin reiniciar
- ✅ Experimentar con diferentes valores
- ✅ Editar materiales desde el Material Editor

**¡Disfruta desarrollando con DirectX 12!** 🎮✨

---

**Autor**: Eddi Andreé Salazar Matos 🇵🇪  
**Proyecto**: DirectX 12 Research - RTX 3060 12GB  
**Última actualización**: 2024

