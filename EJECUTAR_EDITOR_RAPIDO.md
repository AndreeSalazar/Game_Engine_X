# ⚡ Ejecutar Editor C# - Guía Rápida

## 🎯 El Problema
Compilaste pero **no ves la ventana del editor**. Esto es normal porque necesitas ejecutarlo manualmente.

---

## ✅ SOLUCIÓN RÁPIDA (3 Pasos)

### Paso 1: Compilar el Proyecto C#
1. En Visual Studio, ve a **Build → Build Solution** (F7)
2. Espera a que termine (debe decir "Build succeeded")

### Paso 2: Ejecutar el Editor
**Opción A - Desde Visual Studio:**
1. En **Solution Explorer**, click derecho en **`DirectX12Editor`**
2. Selecciona **"Set as StartUp Project"**
3. Presiona **F5** (o click en ▶ Start)

**Opción B - Ejecutar .exe Directamente:**
1. Abre **File Explorer** (Explorador de archivos)
2. Ve a: `DirectX Test\Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\`
3. Doble click en **`DirectX12Editor.exe`**

### Paso 3: Verificar
Deberías ver **DOS ventanas**:
- ✅ **DirectX12Test.exe** (cubo 3D rotando) - Ya lo tienes
- ✅ **DirectX 12 Editor** (controles y sliders) - Esta es la que falta

---

## 🎮 Flujo Completo de Trabajo

### Para Usar el Sistema Completo:

1. **Ejecuta DirectX12Test.exe** (F5 en Visual Studio)
   - Verás el cubo 3D rotando

2. **Ejecuta DirectX12Editor.exe** (F5 o doble click)
   - Verás la ventana con controles

3. **Ajusta los sliders** en DirectX12Editor
   - Los cambios se aplican en tiempo real al cubo

---

## 🔍 Verificar que Funciona

### En DirectX12Editor deberías ver:
- ✅ Panel izquierdo: "Details Panel" con sliders
- ✅ Panel central: "DirectX 12 Viewport" con información
- ✅ Panel derecho: "Statistics" con valores actuales
- ✅ Botón verde: "🎨 Open Material Editor"

### Si no aparece:
1. Verifica que compiló sin errores
2. Revisa la ventana **Output** en Visual Studio
3. Busca errores en **Error List**

---

## 💡 Tip Importante

**Visual Studio puede ejecutar solo UN proyecto a la vez por defecto.**

Para ejecutar ambos simultáneamente:
- **Opción 1**: Ejecuta DirectX12Test.exe desde Visual Studio (F5)
- **Opción 2**: Ejecuta DirectX12Editor.exe desde File Explorer (doble click)

O usa **dos instancias de Visual Studio**:
- Instancia 1: Ejecuta DirectX12Test
- Instancia 2: Ejecuta DirectX12Editor

---

## 🐛 Si Aún No Funciona

### Checklist:
- [ ] ¿Compiló sin errores? (Build → Build Solution)
- [ ] ¿El .exe existe? (`Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe`)
- [ ] ¿Presionaste F5 o ejecutaste el .exe?
- [ ] ¿Hay errores en Output o Error List?

### Errores Comunes:
- **"No se puede encontrar el archivo"**: Compila primero (F7)
- **"Newtonsoft.Json no encontrado"**: Restaura NuGet packages
- **"No se puede iniciar"**: Verifica que .NET 8.0 esté instalado

---

**¡El ejecutable existe! Solo necesitas ejecutarlo manualmente.** 🚀

