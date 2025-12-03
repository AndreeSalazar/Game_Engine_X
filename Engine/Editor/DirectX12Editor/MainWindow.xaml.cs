using System;
using System.Diagnostics;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;
using System.Timers;

namespace DirectX12Editor
{
    public partial class MainWindow : Window
    {
        private string? configPath = "config.json";
        private Timer updateTimer;
        private bool isUpdating = false;
        private DirectXHost? directXHost;

        public MainWindow()
        {
            InitializeComponent();
            
            // Configurar ruta del archivo de configuración
            // Buscar en el directorio del ejecutable de DirectX 12
            // Buscar config.json en múltiples ubicaciones (nueva estructura)
            string[] possiblePaths = {
                Path.Combine("Engine", "Binaries", "Win64", "config.json"),
                Path.Combine("x64", "Debug", "config.json"),
                "config.json"
            };
            
            configPath = null;
            foreach (string path in possiblePaths)
            {
                if (File.Exists(path))
                {
                    configPath = path;
                    break;
                }
            }
            
            // Si no existe, usar la primera ruta y crear el directorio
            if (configPath == null)
            {
                configPath = possiblePaths[0];
                string? dir = Path.GetDirectoryName(configPath);
                if (dir != null && !Directory.Exists(dir))
                {
                    Directory.CreateDirectory(dir);
                }
            }
            
            if (ConfigPathTextBlock != null)
            {
                ConfigPathTextBlock.Text = $"Config: {configPath}";
            }
            
            // Timer para actualizar estadísticas cada segundo
            updateTimer = new Timer(1000);
            updateTimer.Elapsed += UpdateTimer_Elapsed;
            updateTimer.Start();
            
            // Cargar configuración inicial DESPUÉS de que los controles estén inicializados
            // Usar Dispatcher para asegurar que la UI esté lista
            Dispatcher.BeginInvoke(new Action(() =>
            {
                LoadConfig();
                UpdateCurrentValues();
                
                // Inicializar DirectX Host
                InitializeDirectXHost();
                
                // Actualizar estado inicial
                if (StatusTextBlock != null)
                {
                    StatusTextBlock.Text = "Estado: Editor listo - Ajusta controles en tiempo real";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
                }
                
                // Verificar si DirectX12Test.exe está ejecutándose
                CheckDirectX12Process();
            }), System.Windows.Threading.DispatcherPriority.Loaded);
            
            // Manejar cambio de tamaño para actualizar el viewport
            this.SizeChanged += MainWindow_SizeChanged;
        }

        private void CheckDirectX12Process()
        {
            if (StatusTextBlock == null) return;
            
            try
            {
                System.Diagnostics.Process[] processes = System.Diagnostics.Process.GetProcessesByName("DirectX12Test");
                if (processes.Length > 0)
                {
                    StatusTextBlock.Text = "Estado: ✅ DirectX12Test.exe ejecutándose";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
                }
                else
                {
                    StatusTextBlock.Text = "Estado: ⚠ Ejecuta DirectX12Test.exe para ver cambios";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.Orange;
                }
            }
            catch
            {
                // Ignorar errores
            }
        }

        private void UpdateTimer_Elapsed(object? sender, ElapsedEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                UpdateCurrentValues();
                if (LastUpdateTextBlock != null)
                {
                    LastUpdateTextBlock.Text = $"Última actualización: {DateTime.Now:HH:mm:ss}";
                }
            });
        }

        private void UpdateCurrentValues()
        {
            // Verificar que los controles estén inicializados
            if (CurrentRotationSpeedText == null || RotationSpeedSlider == null ||
                CurrentScaleText == null || ScaleSlider == null ||
                CurrentCameraText == null || CameraXTextBox == null ||
                CameraYTextBox == null || CameraZTextBox == null ||
                CurrentFOVText == null || FOVSlider == null)
            {
                return; // Los controles aún no están listos
            }
            
            CurrentRotationSpeedText.Text = RotationSpeedSlider.Value.ToString("F3");
            CurrentScaleText.Text = ScaleSlider.Value.ToString("F2");
            CurrentCameraText.Text = $"({CameraXTextBox.Text}, {CameraYTextBox.Text}, {CameraZTextBox.Text})";
            CurrentFOVText.Text = FOVSlider.Value.ToString("F3");
        }

        private void SaveConfig()
        {
            if (isUpdating) return;
            
            // Verificar que los controles estén inicializados
            if (RotationSpeedSlider == null || ScaleSlider == null || 
                RotationXMultiplierSlider == null || CameraXTextBox == null ||
                CameraYTextBox == null || CameraZTextBox == null ||
                FOVSlider == null || AutoRotateCheckBox == null)
            {
                return; // Los controles aún no están listos
            }
            
            try
            {
                var config = new
                {
                    rotationSpeed = RotationSpeedSlider.Value,
                    scale = ScaleSlider.Value,
                    rotationXMultiplier = RotationXMultiplierSlider.Value,
                    cameraX = ParseFloat(CameraXTextBox.Text, 0.0f),
                    cameraY = ParseFloat(CameraYTextBox.Text, 1.0f),
                    cameraZ = ParseFloat(CameraZTextBox.Text, -4.0f),
                    fov = FOVSlider.Value,
                    autoRotate = AutoRotateCheckBox.IsChecked ?? true,
                    clearColorR = 0.05f,
                    clearColorG = 0.05f,
                    clearColorB = 0.1f
                };

                string json = JsonConvert.SerializeObject(config, Formatting.Indented);
                
                // Verificar que configPath no sea null
                if (configPath == null)
                {
                    if (StatusTextBlock != null)
                    {
                        StatusTextBlock.Text = "Error: Ruta de configuración no válida";
                        StatusTextBlock.Foreground = System.Windows.Media.Brushes.Red;
                    }
                    return;
                }
                
                // Asegurar que el directorio existe
                string? dir = Path.GetDirectoryName(configPath);
                if (!string.IsNullOrEmpty(dir) && !Directory.Exists(dir))
                {
                    Directory.CreateDirectory(dir);
                }
                
                File.WriteAllText(configPath, json);
                
                if (StatusTextBlock != null)
                {
                    StatusTextBlock.Text = "Estado: Configuración guardada ✓";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
                }
                
                UpdateCurrentValues();
            }
            catch (Exception ex)
            {
                if (StatusTextBlock != null)
                {
                    StatusTextBlock.Text = $"Error: {ex.Message}";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.Red;
                }
            }
        }

        private void LoadConfig()
        {
            // Verificar que los controles estén inicializados
            if (RotationSpeedSlider == null || ScaleSlider == null ||
                RotationXMultiplierSlider == null || CameraXTextBox == null ||
                CameraYTextBox == null || CameraZTextBox == null ||
                FOVSlider == null || AutoRotateCheckBox == null ||
                StatusTextBlock == null)
            {
                return; // Los controles aún no están listos
            }
            
            try
            {
                if (configPath != null && File.Exists(configPath))
                {
                    string json = File.ReadAllText(configPath);
                    var config = JsonConvert.DeserializeObject<dynamic>(json);
                    
                    if (config != null)
                    {
                        isUpdating = true;
                        
                        if (config.rotationSpeed != null)
                            RotationSpeedSlider.Value = (double)config.rotationSpeed;
                        if (config.scale != null)
                            ScaleSlider.Value = (double)config.scale;
                        if (config.rotationXMultiplier != null)
                            RotationXMultiplierSlider.Value = (double)config.rotationXMultiplier;
                        if (config.cameraX != null)
                            CameraXTextBox.Text = config.cameraX.ToString();
                        if (config.cameraY != null)
                            CameraYTextBox.Text = config.cameraY.ToString();
                        if (config.cameraZ != null)
                            CameraZTextBox.Text = config.cameraZ.ToString();
                        if (config.fov != null)
                            FOVSlider.Value = (double)config.fov;
                        if (config.autoRotate != null)
                            AutoRotateCheckBox.IsChecked = (bool)config.autoRotate;
                        
                        isUpdating = false;
                        
                        if (StatusTextBlock != null)
                        {
                            StatusTextBlock.Text = "Estado: Configuración cargada ✓";
                            StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
                        }
                    }
                }
                else
                {
                    // Crear configuración por defecto
                    SaveConfig();
                }
            }
            catch (Exception ex)
            {
                if (StatusTextBlock != null)
                {
                    StatusTextBlock.Text = $"Error cargando: {ex.Message}";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.Orange;
                }
            }
        }

        private float ParseFloat(string text, float defaultValue)
        {
            if (float.TryParse(text, out float result))
                return result;
            return defaultValue;
        }

        // Event handlers para controles
        private void RotationSpeedSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isUpdating && RotationSpeedSlider != null)
                SaveConfig();
        }

        private void RotationSpeedTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && RotationSpeedTextBox != null && RotationSpeedSlider != null &&
                float.TryParse(RotationSpeedTextBox.Text, out float value))
            {
                if (value >= RotationSpeedSlider.Minimum && value <= RotationSpeedSlider.Maximum)
                {
                    isUpdating = true;
                    RotationSpeedSlider.Value = value;
                    isUpdating = false;
                    SaveConfig();
                }
            }
        }

        private void ScaleSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isUpdating && ScaleSlider != null)
                SaveConfig();
        }

        private void ScaleTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && ScaleTextBox != null && ScaleSlider != null &&
                float.TryParse(ScaleTextBox.Text, out float value))
            {
                if (value >= ScaleSlider.Minimum && value <= ScaleSlider.Maximum)
                {
                    isUpdating = true;
                    ScaleSlider.Value = value;
                    isUpdating = false;
                    SaveConfig();
                }
            }
        }

        private void RotationXMultiplierSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isUpdating && RotationXMultiplierSlider != null)
                SaveConfig();
        }

        private void RotationXMultiplierTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && RotationXMultiplierTextBox != null && RotationXMultiplierSlider != null &&
                float.TryParse(RotationXMultiplierTextBox.Text, out float value))
            {
                if (value >= RotationXMultiplierSlider.Minimum && value <= RotationXMultiplierSlider.Maximum)
                {
                    isUpdating = true;
                    RotationXMultiplierSlider.Value = value;
                    isUpdating = false;
                    SaveConfig();
                }
            }
        }

        private void CameraXTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && CameraXTextBox != null)
                SaveConfig();
        }

        private void CameraYTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && CameraYTextBox != null)
                SaveConfig();
        }

        private void CameraZTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && CameraZTextBox != null)
                SaveConfig();
        }

        private void FOVSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isUpdating && FOVSlider != null)
                SaveConfig();
        }

        private void FOVTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && FOVTextBox != null && FOVSlider != null &&
                float.TryParse(FOVTextBox.Text, out float value))
            {
                if (value >= FOVSlider.Minimum && value <= FOVSlider.Maximum)
                {
                    isUpdating = true;
                    FOVSlider.Value = value;
                    isUpdating = false;
                    SaveConfig();
                }
            }
        }

        private void AutoRotateCheckBox_Changed(object sender, RoutedEventArgs e)
        {
            if (!isUpdating && AutoRotateCheckBox != null)
                SaveConfig();
        }

        private void ResetButton_Click(object sender, RoutedEventArgs e)
        {
            // Verificar que los controles estén inicializados
            if (RotationSpeedSlider == null || ScaleSlider == null ||
                RotationXMultiplierSlider == null || CameraXTextBox == null ||
                CameraYTextBox == null || CameraZTextBox == null ||
                FOVSlider == null || AutoRotateCheckBox == null)
            {
                return;
            }
            
            isUpdating = true;
            
            RotationSpeedSlider.Value = 0.015;
            ScaleSlider.Value = 0.6;
            RotationXMultiplierSlider.Value = 0.7;
            CameraXTextBox.Text = "0.0";
            CameraYTextBox.Text = "1.0";
            CameraZTextBox.Text = "-4.0";
            FOVSlider.Value = 0.785398f; // XM_PIDIV4
            AutoRotateCheckBox.IsChecked = true;
            
            isUpdating = false;
            SaveConfig();
        }

        private void SaveConfigButton_Click(object sender, RoutedEventArgs e)
        {
            SaveConfig();
        }

        private void LoadConfigButton_Click(object sender, RoutedEventArgs e)
        {
            LoadConfig();
        }

        private void OpenMaterialEditor_Click(object sender, RoutedEventArgs e)
        {
            if (StatusTextBlock == null) return;
            
            try
            {
                // Verificar si Material Editor ya está abierto
                foreach (Window window in Application.Current.Windows)
                {
                    if (window is MaterialEditor)
                    {
                        window.Activate();
                        window.Focus();
                        StatusTextBlock.Text = "Estado: Material Editor ya está abierto";
                        return;
                    }
                }
                
                // Abrir Material Editor
                MaterialEditor materialEditor = new MaterialEditor();
                materialEditor.Owner = this; // Establecer esta ventana como owner
                materialEditor.Show();
                
                if (StatusTextBlock != null)
                {
                    StatusTextBlock.Text = "Estado: ✅ Material Editor abierto";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
                }
            }
            catch (Exception ex)
            {
                MessageBox.Show($"Error abriendo Material Editor: {ex.Message}", "Error", 
                              MessageBoxButton.OK, MessageBoxImage.Error);
                if (StatusTextBlock != null)
                {
                    StatusTextBlock.Text = $"Error: {ex.Message}";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.Red;
                }
            }
        }

        private void InitializeDirectXHost()
        {
            try
            {
                // Crear el host de DirectX
                directXHost = new DirectXHost();
                
                // Agregar al contenedor
                if (DirectXHostContainer != null)
                {
                    DirectXHostContainer.Content = directXHost;
                    DirectXHostContainer.Visibility = Visibility.Visible;
                    
                    // Esperar un momento para que el host se inicialice completamente
                    // Luego actualizar el tamaño para que se ajuste correctamente
                    Dispatcher.BeginInvoke(new System.Action(() =>
                    {
                        if (DirectXHostContainer != null && directXHost != null)
                        {
                            // Obtener el tamaño actual del contenedor
                            double width = DirectXHostContainer.ActualWidth;
                            double height = DirectXHostContainer.ActualHeight;
                            
                            // Si el tamaño es válido, actualizar el viewport
                            if (width > 0 && height > 0)
                            {
                                directXHost.UpdateSize((int)width, (int)height);
                            }
                            else
                            {
                                // Si aún no tiene tamaño, usar un tamaño por defecto razonable
                                directXHost.UpdateSize(800, 600);
                            }
                        }
                    }), System.Windows.Threading.DispatcherPriority.Loaded);
                    
                    // Ocultar el overlay cuando el host esté listo
                    if (ViewportOverlay != null)
                    {
                        ViewportOverlay.Visibility = Visibility.Collapsed;
                    }
                    
                    // Actualizar estado
                    if (StatusTextBlock != null)
                    {
                        StatusTextBlock.Text = "Estado: ✅ DirectX 12 Viewport activo";
                        StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
                    }
                }
            }
            catch (Exception ex)
            {
                // Si falla, mostrar mensaje de error pero continuar
                if (StatusTextBlock != null)
                {
                    StatusTextBlock.Text = $"Estado: ⚠️ Error iniciando viewport: {ex.Message}";
                    StatusTextBlock.Foreground = System.Windows.Media.Brushes.Orange;
                }
                
                // Mostrar el overlay de nuevo
                if (ViewportOverlay != null)
                {
                    ViewportOverlay.Visibility = Visibility.Visible;
                }
            }
        }

        private void MainWindow_SizeChanged(object sender, SizeChangedEventArgs e)
        {
            // Actualizar tamaño del viewport cuando cambie el tamaño de la ventana
            if (directXHost != null && DirectXHostContainer != null)
            {
                // Usar Dispatcher para asegurar que el layout se haya actualizado completamente
                Dispatcher.BeginInvoke(new System.Action(() =>
                {
                    if (directXHost != null && DirectXHostContainer != null)
                    {
                        // Verificar que el contenedor tenga un tamaño válido
                        double width = DirectXHostContainer.ActualWidth;
                        double height = DirectXHostContainer.ActualHeight;
                        
                        if (width > 0 && height > 0)
                        {
                            try
                            {
                                // Actualizar el tamaño del viewport
                                directXHost.UpdateSize((int)width, (int)height);
                            }
                            catch (Exception ex)
                            {
                                // Log del error pero no detener la aplicación
                                Debug.WriteLine($"Error actualizando tamaño del viewport: {ex.Message}");
                            }
                        }
                    }
                }), System.Windows.Threading.DispatcherPriority.Background);
            }
        }

        protected override void OnClosed(EventArgs e)
        {
            updateTimer?.Stop();
            updateTimer?.Dispose();
            
            // Limpiar el host de DirectX
            if (directXHost != null)
            {
                // El host se limpiará automáticamente al destruirse
                directXHost = null;
            }
            
            base.OnClosed(e);
        }
    }
}

