using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace DirectX12Editor
{
    /// <summary>
    /// HwndHost personalizado para incrustar la ventana de DirectX 12 dentro de WPF
    /// </summary>
    public class DirectXHost : HwndHost
    {
        private IntPtr hwndHost;
        private Process? directXProcess;
        private const int WS_CHILD = 0x40000000;
        private const int WS_VISIBLE = 0x10000000;
        private const int WS_CLIPCHILDREN = 0x02000000;
        private const int WS_CLIPSIBLINGS = 0x04000000;

        [DllImport("user32.dll", SetLastError = true, CharSet = CharSet.Auto)]
        private static extern IntPtr CreateWindowEx(
            int dwExStyle,
            string lpClassName,
            string lpWindowName,
            int dwStyle,
            int x,
            int y,
            int nWidth,
            int nHeight,
            IntPtr hWndParent,
            IntPtr hMenu,
            IntPtr hInstance,
            IntPtr lpParam);

        [DllImport("kernel32.dll")]
        private static extern IntPtr GetModuleHandle(string lpModuleName);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool DestroyWindow(IntPtr hWnd);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern IntPtr FindWindow(string lpClassName, string lpWindowName);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool SetParent(IntPtr hWndChild, IntPtr hWndNewParent);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern int SetWindowLong(IntPtr hWnd, int nIndex, int dwNewLong);

        [DllImport("user32.dll", SetLastError = true)]
        private static extern bool SetWindowPos(IntPtr hWnd, IntPtr hWndInsertAfter, int X, int Y, int cx, int cy, uint uFlags);

        private const int GWL_STYLE = -16;
        private const uint SWP_NOZORDER = 0x0004;
        private const uint SWP_NOACTIVATE = 0x0010;
        private const uint SWP_NOMOVE = 0x0002;
        private const uint SWP_NOSIZE = 0x0001;

        protected override HandleRef BuildWindowCore(HandleRef hwndParent)
        {
            // Crear una ventana nativa que servirá como contenedor
            IntPtr hInstance = GetModuleHandle(null);
            
            hwndHost = CreateWindowEx(
                0,
                "STATIC", // Clase estática simple
                "",
                WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                0, 0, 100, 100,
                hwndParent.Handle,
                IntPtr.Zero,
                hInstance,
                IntPtr.Zero);

            if (hwndHost == IntPtr.Zero)
            {
                int error = Marshal.GetLastWin32Error();
                throw new InvalidOperationException($"No se pudo crear la ventana host. Error: {error}");
            }

            // Iniciar DirectX12Test.exe y pasarle el HWND padre
            StartDirectXProcess(hwndHost);

            return new HandleRef(this, hwndHost);
        }

        protected override void DestroyWindowCore(HandleRef hwnd)
        {
            // Cerrar el proceso de DirectX si está ejecutándose
            if (directXProcess != null && !directXProcess.HasExited)
            {
                try
                {
                    directXProcess.Kill();
                    directXProcess.WaitForExit(1000);
                }
                catch { }
                directXProcess.Dispose();
            }

            if (hwnd.Handle != IntPtr.Zero)
            {
                DestroyWindow(hwnd.Handle);
            }
        }

        protected override IntPtr WndProc(IntPtr hwnd, int msg, IntPtr wParam, IntPtr lParam, ref bool handled)
        {
            // Manejar mensajes de Windows si es necesario
            return base.WndProc(hwnd, msg, wParam, lParam, ref handled);
        }

        private void StartDirectXProcess(IntPtr parentHwnd)
        {
            try
            {
                // Obtener la ubicación del ensamblado actual (donde está el .exe del editor)
                string? assemblyLocation = System.Reflection.Assembly.GetExecutingAssembly().Location;
                string? assemblyDir = assemblyLocation != null ? Path.GetDirectoryName(assemblyLocation) : null;
                
                // Calcular la raíz del proyecto subiendo desde bin\Debug\net8.0-windows\
                // Estructura: DirectX Test\Engine\Editor\DirectX12Editor\bin\Debug\net8.0-windows\DirectX12Editor.exe
                // Necesitamos llegar a: DirectX Test\x64\Debug\DirectX12Test.exe
                string? projectRoot = assemblyDir;
                if (projectRoot != null)
                {
                    // Subir 5 niveles: net8.0-windows -> Debug -> bin -> DirectX12Editor -> Editor -> Engine
                    for (int i = 0; i < 5 && projectRoot != null; i++)
                    {
                        projectRoot = Path.GetDirectoryName(projectRoot);
                    }
                    // Ahora estamos en "DirectX Test", subir un nivel más para llegar a la raíz
                    if (projectRoot != null)
                    {
                        projectRoot = Path.GetDirectoryName(projectRoot);
                    }
                }

                // Buscar el ejecutable de DirectX12Test en múltiples ubicaciones
                List<string> possiblePaths = new List<string>();
                
                // Desde la raíz del proyecto calculada
                if (projectRoot != null)
                {
                    possiblePaths.Add(Path.Combine(projectRoot, "x64", "Debug", "DirectX12Test.exe"));
                    possiblePaths.Add(Path.Combine(projectRoot, "..", "x64", "Debug", "DirectX12Test.exe"));
                }
                
                // Desde el directorio actual de trabajo
                possiblePaths.Add(Path.Combine(Directory.GetCurrentDirectory(), "x64", "Debug", "DirectX12Test.exe"));
                possiblePaths.Add(Path.Combine("x64", "Debug", "DirectX12Test.exe"));
                possiblePaths.Add(Path.Combine("..", "..", "..", "..", "..", "x64", "Debug", "DirectX12Test.exe"));
                possiblePaths.Add("DirectX12Test.exe");

                string? exePath = null;
                foreach (var path in possiblePaths)
                {
                    string fullPath = Path.IsPathRooted(path) ? path : Path.GetFullPath(path);
                    if (File.Exists(fullPath))
                    {
                        exePath = fullPath;
                        break;
                    }
                }

                if (exePath == null)
                {
                    // Construir mensaje de error con todas las rutas intentadas
                    string searchedPaths = string.Join("\n", possiblePaths.Select(p => 
                        Path.IsPathRooted(p) ? p : Path.GetFullPath(p)));
                    
                    System.Windows.MessageBox.Show(
                        "No se encontró DirectX12Test.exe.\n\n" +
                        "Asegúrate de compilar el proyecto C++ primero (F7 en Visual Studio).\n\n" +
                        "Buscado en:\n" + searchedPaths + "\n\n" +
                        "Ubicación esperada: [Raíz del proyecto]\\x64\\Debug\\DirectX12Test.exe",
                        "Error",
                        System.Windows.MessageBoxButton.OK,
                        System.Windows.MessageBoxImage.Warning);
                    return;
                }

                // Iniciar el proceso con el HWND padre como argumento
                ProcessStartInfo startInfo = new ProcessStartInfo
                {
                    FileName = exePath,
                    Arguments = $"--parent-hwnd {parentHwnd.ToInt64()}",
                    UseShellExecute = false,
                    CreateNoWindow = false, // Cambiar a false para evitar problemas con el dispatcher
                    RedirectStandardOutput = false,
                    RedirectStandardError = false,
                    WorkingDirectory = Path.GetDirectoryName(exePath) ?? Directory.GetCurrentDirectory()
                };

                // Iniciar el proceso directamente (sin dispatcher para evitar problemas)
                try
                {
                    directXProcess = Process.Start(startInfo);
                }
                catch (Exception ex)
                {
                    System.Windows.MessageBox.Show(
                        $"Error al iniciar DirectX12Test.exe:\n{ex.Message}\n\n" +
                        $"Ruta: {exePath}",
                        "Error",
                        System.Windows.MessageBoxButton.OK,
                        System.Windows.MessageBoxImage.Error);
                    return;
                }

                if (directXProcess != null && startInfo.FileName != null)
                {
                    // Esperar a que la ventana se cree (máximo 5 segundos)
                    IntPtr directXHwnd = IntPtr.Zero;
                    int attempts = 0;
                    const int maxAttempts = 50; // 5 segundos con esperas de 100ms
                    
                    while (directXHwnd == IntPtr.Zero && attempts < maxAttempts && !directXProcess.HasExited)
                    {
                        System.Threading.Thread.Sleep(100);
                        directXHwnd = FindWindow("DirectX12WindowClass", "DirectX 12 - RTX 3060 12GB");
                        attempts++;
                    }

                    if (directXHwnd != IntPtr.Zero)
                    {
                        // Establecer como ventana hijo
                        SetParent(directXHwnd, parentHwnd);

                        // Cambiar el estilo para que sea una ventana hijo
                        int style = GetWindowLong(directXHwnd, GWL_STYLE);
                        style = (style & ~0x00CF0000) | WS_CHILD | WS_VISIBLE;
                        SetWindowLong(directXHwnd, GWL_STYLE, style);

                        // Obtener el tamaño del contenedor padre
                        RECT rect;
                        GetClientRect(parentHwnd, out rect);
                        int width = rect.right - rect.left;
                        int height = rect.bottom - rect.top;
                        
                        // Asegurar tamaño mínimo válido
                        if (width <= 0) width = 800;
                        if (height <= 0) height = 600;

                        // Ajustar posición y tamaño para llenar el contenedor
                        SetWindowPos(directXHwnd, IntPtr.Zero, 0, 0, width, height,
                            SWP_NOZORDER | SWP_NOACTIVATE);
                        
                        // Enviar mensaje WM_SIZE para que DirectX inicialice con el tamaño correcto
                        IntPtr wParam = new IntPtr(SIZE_RESTORED);
                        IntPtr lParam = new IntPtr((height << 16) | (width & 0xFFFF));
                        PostMessage(directXHwnd, WM_SIZE, wParam, lParam);
                    }
                }
            }
            catch (Exception ex)
            {
                System.Windows.MessageBox.Show(
                    $"Error al iniciar DirectX12Test.exe:\n{ex.Message}",
                    "Error",
                    System.Windows.MessageBoxButton.OK,
                    System.Windows.MessageBoxImage.Error);
            }
        }

        [StructLayout(LayoutKind.Sequential)]
        private struct RECT
        {
            public int left;
            public int top;
            public int right;
            public int bottom;
        }

        [DllImport("user32.dll")]
        private static extern bool GetClientRect(IntPtr hWnd, out RECT lpRect);

        [DllImport("user32.dll")]
        private static extern int GetWindowLong(IntPtr hWnd, int nIndex);

        [DllImport("user32.dll")]
        private static extern bool PostMessage(IntPtr hWnd, uint Msg, IntPtr wParam, IntPtr lParam);

        private const uint WM_SIZE = 0x0005;
        private const uint SIZE_RESTORED = 0;

        public void UpdateSize(int width, int height)
        {
            if (hwndHost != IntPtr.Zero && width > 0 && height > 0)
            {
                // Actualizar tamaño del host
                SetWindowPos(hwndHost, IntPtr.Zero, 0, 0, width, height,
                    SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);

                // También actualizar la ventana de DirectX si existe
                IntPtr directXHwnd = FindWindow("DirectX12WindowClass", "DirectX 12 - RTX 3060 12GB");
                if (directXHwnd != IntPtr.Zero)
                {
                    // Actualizar tamaño de la ventana
                    SetWindowPos(directXHwnd, IntPtr.Zero, 0, 0, width, height,
                        SWP_NOZORDER | SWP_NOACTIVATE | SWP_NOMOVE);
                    
                    // Enviar mensaje WM_SIZE para que DirectX actualice el viewport
                    IntPtr wParam = new IntPtr(SIZE_RESTORED);
                    IntPtr lParam = new IntPtr((height << 16) | (width & 0xFFFF));
                    PostMessage(directXHwnd, WM_SIZE, wParam, lParam);
                }
            }
        }
    }
}

