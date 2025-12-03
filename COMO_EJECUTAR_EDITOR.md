# 🚀 Cómo Ejecutar el Editor C# - Guía Rápida

## ⚠️ Problema Común: No se ve la UI

Si compilaste pero no ves la ventana del editor, sigue estos pasos:

---

## ✅ Solución 1: Ejecutar desde Visual Studio

### Paso 1: Configurar Proyecto de Inicio
1. En **Solution Explorer**, busca el proyecto `DirectX12Editor`
2. **Click derecho** en `DirectX12Editor`
3. Selecciona **"Set as StartUp Project"** (Establecer como proyecto de inicio)
4. Verás que el nombre del proyecto se pone en **negrita**

### Paso 2: Ejecutar
1. Presiona **F5** (Start Debugging)
2. O click en el botón **▶ Start** (verde) en la barra de herramientas
3. Debería abrirse la ventana **"DirectX 12 Editor - RTX 3060 12GB"**

---

## ✅ Solución 2: Ejecutar Directamente el .exe

### Ubicación del Ejecutable:
```
Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe
```

### Pasos:
1. Abre **File Explorer** (Explorador de archivos)
2. Navega a: `DirectX Test\Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\`
3. Doble click en **DirectX12Editor.exe**
4. Se abrirá la ventana del editor

---

## ✅ Solución 3: Ejecutar Ambos (Editor + DirectX12Test.exe)

### Opción A: Dos Instancias de Visual Studio
1. **Primera instancia**: Ejecuta `DirectX12Test` (F5)
2. **Segunda instancia**: 
   - Abre otra ventana de Visual Studio
   - Abre el mismo proyecto
   - Ejecuta `DirectX12Editor` (F5)

### Opción B: Ejecutar .exe Manualmente
1. Ejecuta `DirectX12Test.exe` desde Visual Studio (F5)
2. Ejecuta `DirectX12Editor.exe` desde File Explorer (doble click)

---

## 🔍 Verificar que Compiló Correctamente

### En Visual Studio:
1. **Build → Build Solution** (F7)
2. Revisa la ventana **Output**:
   - Debe decir: `Build succeeded`
   - No debe haber errores

### Si hay errores:
- Revisa la ventana **Error List**
- Los errores más comunes:
  - Falta `Newtonsoft.Json` → Instalar NuGet package
  - Rutas incorrectas → Verificar que los archivos existan

---

## 📋 Checklist de Verificación

- [ ] Proyecto `DirectX12Editor` compila sin errores
- [ ] `DirectX12Editor.exe` existe en `bin\Debug\net8.0-windows\`
- [ ] Proyecto está configurado como "StartUp Project"
- [ ] Presionaste F5 o ejecutaste el .exe directamente
- [ ] No hay errores en la ventana Output

---

## 🎯 Flujo de Trabajo Recomendado

### Para Desarrollo:
1. **Abre Visual Studio**
2. **Ejecuta DirectX12Test** (F5) - Para ver el cubo 3D
3. **Ejecuta DirectX12Editor** (F5) - Para controlar el cubo
   - O ejecuta el .exe manualmente si prefieres

### Resultado Esperado:
- **Ventana 1**: DirectX12Test.exe (cubo 3D rotando)
- **Ventana 2**: DirectX12Editor (controles y sliders)
- **Cambios en tiempo real**: Ajusta sliders → Cubo cambia

---

## 🐛 Si Aún No Funciona

### Verificar:
1. **¿Compiló correctamente?**
   - Build → Build Solution
   - Revisa errores

2. **¿El .exe existe?**
   - Busca en: `Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\`

3. **¿Hay errores al ejecutar?**
   - Revisa la ventana Output
   - Revisa Event Viewer de Windows

4. **¿.NET Runtime instalado?**
   - El proyecto usa .NET 8.0
   - Verifica que esté instalado

---

## 💡 Tip Rápido

**Atajo Visual Studio:**
- **Ctrl+F5**: Ejecutar sin debugger (más rápido)
- **F5**: Ejecutar con debugger (puedes poner breakpoints)

---

**Autor**: Eddi Andreé Salazar Matos 🇵🇪  
**Proyecto**: DirectX 12 Research - RTX 3060 12GB

