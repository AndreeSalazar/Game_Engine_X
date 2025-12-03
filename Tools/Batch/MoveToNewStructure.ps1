# Script de Migración a Nueva Estructura Estilo Unreal Engine 5
# Autor: Eddi Andreé Salazar Matos 🇵🇪

Write-Host "`n=== MIGRACIÓN A ESTRUCTURA UNREAL ENGINE 5 ===" -ForegroundColor Cyan
Write-Host "`nEste script copiará archivos a la nueva estructura" -ForegroundColor Yellow
Write-Host "Los archivos originales NO se eliminarán hasta verificar que todo funciona`n" -ForegroundColor Yellow

# Confirmar
$confirm = Read-Host "¿Continuar? (S/N)"
if ($confirm -ne "S" -and $confirm -ne "s") {
    Write-Host "Migración cancelada" -ForegroundColor Red
    exit
}

# Crear estructura si no existe
Write-Host "`nCreando estructura de carpetas..." -ForegroundColor Cyan
$folders = @(
    "Engine\Core\Include",
    "Engine\Core\Source",
    "Engine\Rendering\Shaders",
    "Engine\Editor",
    "Engine\Content\Materials",
    "Engine\Content\Textures",
    "Engine\Content\Models",
    "Engine\Config",
    "Engine\Binaries\Win64",
    "Engine\Intermediate\Build",
    "Engine\Intermediate\Shaders"
)

foreach ($folder in $folders) {
    if (!(Test-Path $folder)) {
        New-Item -ItemType Directory -Path $folder -Force | Out-Null
        Write-Host "  ✓ Creado: $folder" -ForegroundColor Green
    } else {
        Write-Host "  → Ya existe: $folder" -ForegroundColor Gray
    }
}

# Copiar archivos (NO mover para seguridad)
Write-Host "`nCopiando archivos..." -ForegroundColor Cyan

# Headers
if (Test-Path "include") {
    $files = Get-ChildItem -Path "include" -Filter "*.h"
    foreach ($file in $files) {
        Copy-Item -Path $file.FullName -Destination "Engine\Core\Include\" -Force
        Write-Host "  ✓ Copiado: $($file.Name)" -ForegroundColor Green
    }
} else {
    Write-Host "  ⚠ Carpeta 'include' no encontrada" -ForegroundColor Yellow
}

# Source
if (Test-Path "src") {
    $files = Get-ChildItem -Path "src" -Filter "*.cpp"
    foreach ($file in $files) {
        Copy-Item -Path $file.FullName -Destination "Engine\Core\Source\" -Force
        Write-Host "  ✓ Copiado: $($file.Name)" -ForegroundColor Green
    }
} else {
    Write-Host "  ⚠ Carpeta 'src' no encontrada" -ForegroundColor Yellow
}

# Shaders
if (Test-Path "shaders") {
    $files = Get-ChildItem -Path "shaders" -Filter "*.hlsl"
    foreach ($file in $files) {
        Copy-Item -Path $file.FullName -Destination "Engine\Rendering\Shaders\" -Force
        Write-Host "  ✓ Copiado: $($file.Name)" -ForegroundColor Green
    }
} else {
    Write-Host "  ⚠ Carpeta 'shaders' no encontrada" -ForegroundColor Yellow
}

# Editor C#
if (Test-Path "DirectX12Editor") {
    Copy-Item -Path "DirectX12Editor" -Destination "Engine\Editor\" -Recurse -Force
    Write-Host "  ✓ Editor C# copiado" -ForegroundColor Green
} else {
    Write-Host "  ⚠ Carpeta 'DirectX12Editor' no encontrada" -ForegroundColor Yellow
}

# Config files
if (Test-Path "x64\Debug\config.json") {
    Copy-Item -Path "x64\Debug\config.json" -Destination "Engine\Binaries\Win64\config.json" -Force
    Write-Host "  ✓ config.json copiado" -ForegroundColor Green
}

Write-Host "`n✅ Migración completada!" -ForegroundColor Green
Write-Host "`nPRÓXIMOS PASOS:" -ForegroundColor Yellow
Write-Host "1. Actualizar CMakeLists.txt con nuevas rutas" -ForegroundColor White
Write-Host "2. Actualizar includes en código C++" -ForegroundColor White
Write-Host "3. Actualizar rutas en código C#" -ForegroundColor White
Write-Host "4. Compilar y probar" -ForegroundColor White
Write-Host "5. Si todo funciona, eliminar carpetas antiguas`n" -ForegroundColor White

