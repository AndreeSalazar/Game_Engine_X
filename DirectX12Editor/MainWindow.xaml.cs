using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using Newtonsoft.Json;
using System.Timers;

namespace DirectX12Editor
{
    public partial class MainWindow : Window
    {
        private string configPath = "config.json";
        private Timer updateTimer;
        private bool isUpdating = false;

        public MainWindow()
        {
            InitializeComponent();
            
            // Configurar ruta del archivo de configuración
            // Buscar en el directorio del ejecutable de DirectX 12
            string? exeDir = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            string? parentDir = exeDir != null ? Directory.GetParent(exeDir)?.FullName : null;
            if (parentDir != null)
            {
                configPath = Path.Combine(parentDir, "x64", "Debug", "config.json");
            }
            else if (exeDir != null)
            {
                configPath = Path.Combine(exeDir, "config.json");
            }
            
            // Si no existe, crear en el directorio actual
            if (!File.Exists(configPath))
            {
                string currentDir = Directory.GetCurrentDirectory();
                configPath = Path.Combine(currentDir, "config.json");
            }
            
            ConfigPathTextBlock.Text = $"Config: {configPath}";
            
            // Cargar configuración inicial
            LoadConfig();
            
            // Timer para actualizar estadísticas cada segundo
            updateTimer = new Timer(1000);
            updateTimer.Elapsed += UpdateTimer_Elapsed;
            updateTimer.Start();
            
            // Actualizar UI inicial
            UpdateCurrentValues();
        }

        private void UpdateTimer_Elapsed(object? sender, ElapsedEventArgs e)
        {
            Dispatcher.Invoke(() =>
            {
                UpdateCurrentValues();
                LastUpdateTextBlock.Text = $"Última actualización: {DateTime.Now:HH:mm:ss}";
            });
        }

        private void UpdateCurrentValues()
        {
            CurrentRotationSpeedText.Text = RotationSpeedSlider.Value.ToString("F3");
            CurrentScaleText.Text = ScaleSlider.Value.ToString("F2");
            CurrentCameraText.Text = $"({CameraXTextBox.Text}, {CameraYTextBox.Text}, {CameraZTextBox.Text})";
            CurrentFOVText.Text = FOVSlider.Value.ToString("F3");
        }

        private void SaveConfig()
        {
            if (isUpdating) return;
            
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
                
                // Asegurar que el directorio existe
                string? dir = Path.GetDirectoryName(configPath);
                if (!string.IsNullOrEmpty(dir) && !Directory.Exists(dir))
                {
                    Directory.CreateDirectory(dir);
                }
                
                File.WriteAllText(configPath, json);
                
                StatusTextBlock.Text = "Estado: Configuración guardada ✓";
                StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
                
                UpdateCurrentValues();
            }
            catch (Exception ex)
            {
                StatusTextBlock.Text = $"Error: {ex.Message}";
                StatusTextBlock.Foreground = System.Windows.Media.Brushes.Red;
            }
        }

        private void LoadConfig()
        {
            try
            {
                if (File.Exists(configPath))
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
                        
                        StatusTextBlock.Text = "Estado: Configuración cargada ✓";
                        StatusTextBlock.Foreground = System.Windows.Media.Brushes.LimeGreen;
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
                StatusTextBlock.Text = $"Error cargando: {ex.Message}";
                StatusTextBlock.Foreground = System.Windows.Media.Brushes.Orange;
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
            if (!isUpdating)
                SaveConfig();
        }

        private void RotationSpeedTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && float.TryParse(RotationSpeedTextBox.Text, out float value))
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
            if (!isUpdating)
                SaveConfig();
        }

        private void ScaleTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && float.TryParse(ScaleTextBox.Text, out float value))
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
            if (!isUpdating)
                SaveConfig();
        }

        private void RotationXMultiplierTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && float.TryParse(RotationXMultiplierTextBox.Text, out float value))
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
            if (!isUpdating)
                SaveConfig();
        }

        private void CameraYTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating)
                SaveConfig();
        }

        private void CameraZTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating)
                SaveConfig();
        }

        private void FOVSlider_ValueChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (!isUpdating)
                SaveConfig();
        }

        private void FOVTextBox_TextChanged(object sender, TextChangedEventArgs e)
        {
            if (!isUpdating && float.TryParse(FOVTextBox.Text, out float value))
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
            if (!isUpdating)
                SaveConfig();
        }

        private void ResetButton_Click(object sender, RoutedEventArgs e)
        {
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

        protected override void OnClosed(EventArgs e)
        {
            updateTimer?.Stop();
            updateTimer?.Dispose();
            base.OnClosed(e);
        }
    }
}

