# Script para ejecutar DirectX12Test.exe (C++) y DirectX12Editor.exe (C#) simultáneamente
# Autor: Eddi Andreé Salazar Matos 🇵🇪

Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  DirectX 12 - Ejecutar Ambos Proyectos" -ForegroundColor Cyan
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""

# Rutas de los ejecutables
$cppExe = "x64\Debug\DirectX12Test.exe"
$csharpExe = "Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe"
$projectRoot = $PSScriptRoot

# Verificar que estamos en el directorio correcto
if (-not (Test-Path $cppExe) -and -not (Test-Path $csharpExe)) {
    Write-Host "Error: No se encontraron los ejecutables." -ForegroundColor Red
    Write-Host "Asegúrate de ejecutar este script desde la raíz del proyecto." -ForegroundColor Yellow
    Write-Host "Directorio actual: $projectRoot" -ForegroundColor Yellow
    exit 1
}

# Verificar y crear directorio para config.json si no existe
$configDir = "Engine\Binaries\Win64"
if (-not (Test-Path $configDir)) {
    Write-Host "Creando directorio para config.json: $configDir" -ForegroundColor Yellow
    New-Item -ItemType Directory -Path $configDir -Force | Out-Null
}

# Función para verificar si un proceso está ejecutándose
function Test-ProcessRunning {
    param([string]$ProcessName)
    $process = Get-Process -Name $ProcessName -ErrorAction SilentlyContinue
    return $null -ne $process
}

# Verificar si ya están ejecutándose
if (Test-ProcessRunning "DirectX12Test") {
    Write-Host "⚠️  DirectX12Test.exe ya está ejecutándose" -ForegroundColor Yellow
    $response = Read-Host "¿Deseas cerrarlo y ejecutar uno nuevo? (S/N)"
    if ($response -eq "S" -or $response -eq "s") {
        Stop-Process -Name "DirectX12Test" -Force -ErrorAction SilentlyContinue
        Start-Sleep -Seconds 1
    } else {
        Write-Host "Manteniendo el proceso existente..." -ForegroundColor Green
    }
}

if (Test-ProcessRunning "DirectX12Editor") {
    Write-Host "⚠️  DirectX12Editor.exe ya está ejecutándose" -ForegroundColor Yellow
    $response = Read-Host "¿Deseas cerrarlo y ejecutar uno nuevo? (S/N)"
    if ($response -eq "S" -or $response -eq "s") {
        Stop-Process -Name "DirectX12Editor" -Force -ErrorAction SilentlyContinue
        Start-Sleep -Seconds 1
    } else {
        Write-Host "Manteniendo el proceso existente..." -ForegroundColor Green
    }
}

# Ejecutar DirectX12Test.exe (C++)
Write-Host ""
Write-Host "🚀 Ejecutando DirectX12Test.exe (C++)..." -ForegroundColor Green
if (Test-Path $cppExe) {
    $cppProcess = Start-Process -FilePath $cppExe -PassThru -WindowStyle Normal
    Write-Host "   ✅ DirectX12Test.exe iniciado (PID: $($cppProcess.Id))" -ForegroundColor Green
} else {
    Write-Host "   ❌ No se encontró: $cppExe" -ForegroundColor Red
    Write-Host "   💡 Compila el proyecto C++ primero (F7 en Visual Studio)" -ForegroundColor Yellow
}

# Esperar un poco antes de ejecutar el editor
Start-Sleep -Seconds 2

# Ejecutar DirectX12Editor.exe (C#)
Write-Host ""
Write-Host "🚀 Ejecutando DirectX12Editor.exe (C#)..." -ForegroundColor Green
if (Test-Path $csharpExe) {
    $csharpProcess = Start-Process -FilePath $csharpExe -PassThru -WindowStyle Normal
    Write-Host "   ✅ DirectX12Editor.exe iniciado (PID: $($csharpProcess.Id))" -ForegroundColor Green
} else {
    Write-Host "   ❌ No se encontró: $csharpExe" -ForegroundColor Red
    Write-Host "   💡 Compila el proyecto C# primero (F7 en Visual Studio)" -ForegroundColor Yellow
}

Write-Host ""
Write-Host "========================================" -ForegroundColor Cyan
Write-Host "  ✅ Ambos proyectos ejecutándose" -ForegroundColor Green
Write-Host "========================================" -ForegroundColor Cyan
Write-Host ""
Write-Host "💡 Tips:" -ForegroundColor Yellow
Write-Host "   - Ajusta los sliders en DirectX12Editor" -ForegroundColor White
Write-Host "   - Los cambios se aplican en tiempo real al cubo 3D" -ForegroundColor White
Write-Host "   - El archivo config.json se guarda en: Engine\Binaries\Win64\" -ForegroundColor White
Write-Host ""
Write-Host "Presiona Ctrl+C para cerrar este script (los procesos seguirán ejecutándose)" -ForegroundColor Gray
Write-Host ""

# Mantener el script ejecutándose para mostrar información
try {
    while ($true) {
        Start-Sleep -Seconds 5
        
        # Verificar si los procesos siguen ejecutándose
        $cppRunning = Test-ProcessRunning "DirectX12Test"
        $csharpRunning = Test-ProcessRunning "DirectX12Editor"
        
        if (-not $cppRunning -and -not $csharpRunning) {
            Write-Host ""
            Write-Host "⚠️  Ambos procesos se han cerrado." -ForegroundColor Yellow
            break
        }
    }
} catch {
    # Ctrl+C presionado
    Write-Host ""
    Write-Host "Script detenido. Los procesos continúan ejecutándose." -ForegroundColor Yellow
}

