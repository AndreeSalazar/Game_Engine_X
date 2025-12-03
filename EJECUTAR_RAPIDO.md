# ⚡ Ejecutar C# + C++ - Guía Rápida

## 🚀 Método Más Rápido (3 Pasos)

### 1️⃣ Compilar Todo
```
Visual Studio → F7 (Build Solution)
```

### 2️⃣ Ejecutar C++ (DirectX12Test)
```
Visual Studio → F5 (con DirectX12Test como StartUp Project)
```

### 3️⃣ Ejecutar C# (DirectX12Editor)
```
File Explorer → Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe
```

**O usar el script PowerShell:**
```powershell
.\EjecutarAmbos.ps1
```

---

## 📁 Archivos Importantes

- **C++ Exe**: `x64\Debug\DirectX12Test.exe`
- **C# Exe**: `Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe`
- **Config**: `Engine\Binaries\Win64\config.json` (se crea automáticamente)

---

## ✅ Verificar que Funciona

1. **DirectX12Test.exe** → Cubo 3D rotando ✅
2. **DirectX12Editor.exe** → Ventana con controles ✅
3. **Mover slider** → Cubo cambia en tiempo real ✅

---

## 🐛 Problemas Comunes

**Cubo no cambia:**
- Verifica que `Engine\Binaries\Win64\config.json` existe
- Verifica que ambas apps están ejecutándose

**No veo el editor:**
- Compila primero (F7)
- Ejecuta manualmente desde File Explorer

---

**Ver guía completa:** `GUIA_EJECUTAR_TIEMPO_REAL.md`

