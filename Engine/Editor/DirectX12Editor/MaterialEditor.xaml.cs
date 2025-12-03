using System;
using System.IO;
using System.Windows;
using System.Windows.Controls;
using System.Windows.Media;
using Newtonsoft.Json;
using System.Diagnostics;

namespace DirectX12Editor
{
    public partial class MaterialEditor : Window
    {
        private string materialsPath = Path.Combine("Engine", "Content", "Materials");
        private string configPath = Path.Combine("Engine", "Binaries", "Win64", "current_material.json");
        private string currentMaterialPath = "";
        private System.Windows.Threading.DispatcherTimer updateTimer;
        private System.Windows.Threading.DispatcherTimer autoSaveTimer;

        public MaterialEditor()
        {
            InitializeComponent();
            
            // Crear directorios si no existen
            if (!Directory.Exists(materialsPath))
            {
                Directory.CreateDirectory(materialsPath);
            }
            
            string binariesPath = Path.Combine("Engine", "Binaries", "Win64");
            if (!Directory.Exists(binariesPath))
            {
                Directory.CreateDirectory(binariesPath);
            }
            
            // Configurar eventos
            BaseColorRSlider.ValueChanged += (s, e) => UpdateMaterial();
            BaseColorGSlider.ValueChanged += (s, e) => UpdateMaterial();
            BaseColorBSlider.ValueChanged += (s, e) => UpdateMaterial();
            MetallicSlider.ValueChanged += (s, e) => UpdateMaterial();
            RoughnessSlider.ValueChanged += (s, e) => UpdateMaterial();
            EmissiveRSlider.ValueChanged += (s, e) => UpdateMaterial();
            EmissiveGSlider.ValueChanged += (s, e) => UpdateMaterial();
            EmissiveBSlider.ValueChanged += (s, e) => UpdateMaterial();
            
            // Sincronizar textboxes con sliders
            SyncTextBoxes();
            
            // Timer para actualizar preview
            updateTimer = new System.Windows.Threading.DispatcherTimer();
            updateTimer.Interval = TimeSpan.FromMilliseconds(100);
            updateTimer.Tick += (s, e) => UpdatePreview();
            updateTimer.Start();
            
            // Timer para auto-guardar material cada 2 segundos
            autoSaveTimer = new System.Windows.Threading.DispatcherTimer();
            autoSaveTimer.Interval = TimeSpan.FromSeconds(2);
            autoSaveTimer.Tick += (s, e) => UpdateMaterial();
            autoSaveTimer.Start();
            
            // Cargar material por defecto si existe
            LoadDefaultMaterial();
            
            // Actualizar título con estado
            this.Title = "Material Editor - Estilo Unreal Engine 5 | Conectado a DirectX12Test.exe";
        }

        private void SyncTextBoxes()
        {
            BaseColorRTextBox.Text = BaseColorRSlider.Value.ToString("F2");
            BaseColorGTextBox.Text = BaseColorGSlider.Value.ToString("F2");
            BaseColorBTextBox.Text = BaseColorBSlider.Value.ToString("F2");
            MetallicTextBox.Text = MetallicSlider.Value.ToString("F2");
            RoughnessTextBox.Text = RoughnessSlider.Value.ToString("F2");
            EmissiveRTextBox.Text = EmissiveRSlider.Value.ToString("F2");
            EmissiveGTextBox.Text = EmissiveGSlider.Value.ToString("F2");
            EmissiveBTextBox.Text = EmissiveBSlider.Value.ToString("F2");
        }

        private void OnParameterChanged(object sender, RoutedPropertyChangedEventArgs<double> e)
        {
            if (sender == BaseColorRSlider) BaseColorRTextBox.Text = e.NewValue.ToString("F2");
            else if (sender == BaseColorGSlider) BaseColorGTextBox.Text = e.NewValue.ToString("F2");
            else if (sender == BaseColorBSlider) BaseColorBTextBox.Text = e.NewValue.ToString("F2");
            else if (sender == MetallicSlider) MetallicTextBox.Text = e.NewValue.ToString("F2");
            else if (sender == RoughnessSlider) RoughnessTextBox.Text = e.NewValue.ToString("F2");
            else if (sender == EmissiveRSlider) EmissiveRTextBox.Text = e.NewValue.ToString("F2");
            else if (sender == EmissiveGSlider) EmissiveGTextBox.Text = e.NewValue.ToString("F2");
            else if (sender == EmissiveBSlider) EmissiveBTextBox.Text = e.NewValue.ToString("F2");
            
            UpdateMaterial();
        }

        private void OnParameterTextChanged(object sender, TextChangedEventArgs e)
        {
            if (sender is TextBox textBox)
            {
                if (double.TryParse(textBox.Text, out double value))
                {
                    if (sender == BaseColorRTextBox) BaseColorRSlider.Value = Math.Clamp(value, 0, 1);
                    else if (sender == BaseColorGTextBox) BaseColorGSlider.Value = Math.Clamp(value, 0, 1);
                    else if (sender == BaseColorBTextBox) BaseColorBSlider.Value = Math.Clamp(value, 0, 1);
                    else if (sender == MetallicTextBox) MetallicSlider.Value = Math.Clamp(value, 0, 1);
                    else if (sender == RoughnessTextBox) RoughnessSlider.Value = Math.Clamp(value, 0, 1);
                    else if (sender == EmissiveRTextBox) EmissiveRSlider.Value = Math.Clamp(value, 0, 10);
                    else if (sender == EmissiveGTextBox) EmissiveGSlider.Value = Math.Clamp(value, 0, 10);
                    else if (sender == EmissiveBTextBox) EmissiveBSlider.Value = Math.Clamp(value, 0, 10);
                    
                    UpdateMaterial();
                }
            }
        }

        private void UpdateMaterial()
        {
            // Crear estructura de material
            var material = new
            {
                name = MaterialNameTextBox.Text,
                shader = new
                {
                    vertexShader = "Engine/Rendering/Shaders/BasicVS.hlsl",
                    pixelShader = "Engine/Rendering/Shaders/BasicPS.hlsl"
                },
                parameters = new
                {
                    BaseColor = new
                    {
                        type = "vector3",
                        value = new[]
                        {
                            BaseColorRSlider.Value,
                            BaseColorGSlider.Value,
                            BaseColorBSlider.Value
                        },
                        displayName = "Base Color"
                    },
                    Metallic = new
                    {
                        type = "scalar",
                        value = MetallicSlider.Value,
                        displayName = "Metallic",
                        min = 0.0,
                        max = 1.0
                    },
                    Roughness = new
                    {
                        type = "scalar",
                        value = RoughnessSlider.Value,
                        displayName = "Roughness",
                        min = 0.0,
                        max = 1.0
                    },
                    Emissive = new
                    {
                        type = "vector3",
                        value = new[]
                        {
                            EmissiveRSlider.Value,
                            EmissiveGSlider.Value,
                            EmissiveBSlider.Value
                        },
                        displayName = "Emissive Color"
                    }
                }
            };

            // Guardar a archivo JSON
            string json = JsonConvert.SerializeObject(material, Formatting.Indented);
            
            // Guardar en directorio de materiales
            string materialFile = Path.Combine(materialsPath, MaterialNameTextBox.Text + ".json");
            File.WriteAllText(materialFile, json);

            // Enviar a DirectX12Test.exe mediante archivo compartido
            SendMaterialToDirectX12(json);
            
            // Actualizar info
            UpdateMaterialInfo(material);
        }

        private void UpdatePreview()
        {
            // Actualizar preview de color
            byte r = (byte)(BaseColorRSlider.Value * 255);
            byte g = (byte)(BaseColorGSlider.Value * 255);
            byte b = (byte)(BaseColorBSlider.Value * 255);
            ColorPreview.Background = new SolidColorBrush(Color.FromRgb(r, g, b));
        }

        private void UpdateMaterialInfo(dynamic material)
        {
            MaterialInfoText.Text = $"Material: {material.name}\n\n" +
                                   $"Base Color: ({material.parameters.BaseColor.value[0]:F2}, {material.parameters.BaseColor.value[1]:F2}, {material.parameters.BaseColor.value[2]:F2})\n" +
                                   $"Metallic: {material.parameters.Metallic.value:F2}\n" +
                                   $"Roughness: {material.parameters.Roughness.value:F2}\n" +
                                   $"Emissive: ({material.parameters.Emissive.value[0]:F2}, {material.parameters.Emissive.value[1]:F2}, {material.parameters.Emissive.value[2]:F2})";
            
            StatsText.Text = $"Parameters: 4\n" +
                            $"Scalars: 2\n" +
                            $"Vectors: 2\n" +
                            $"Textures: 0";
        }

        private void SendMaterialToDirectX12(string materialJson)
        {
            try
            {
                // Guardar en archivo compartido
                File.WriteAllText(configPath, materialJson);
                
                // Actualizar estado en preview
                PreviewStatusText.Text = $"Material aplicado: {DateTime.Now:HH:mm:ss}";
                PreviewStatusText.Foreground = System.Windows.Media.Brushes.LimeGreen;
            }
            catch (Exception ex)
            {
                PreviewStatusText.Text = $"Error: {ex.Message}";
                PreviewStatusText.Foreground = System.Windows.Media.Brushes.Red;
            }
        }

        private void ApplyMaterial_Click(object sender, RoutedEventArgs e)
        {
            UpdateMaterial();
            MessageBox.Show("Material aplicado a DirectX12Test.exe", "Material Editor", 
                          MessageBoxButton.OK, MessageBoxImage.Information);
        }

        private void SaveMaterial_Click(object sender, RoutedEventArgs e)
        {
            var saveDialog = new Microsoft.Win32.SaveFileDialog
            {
                Filter = "Material Files (*.json)|*.json",
                DefaultExt = "json",
                FileName = MaterialNameTextBox.Text,
                InitialDirectory = Path.GetFullPath(materialsPath)
            };

            if (saveDialog.ShowDialog() == true)
            {
                UpdateMaterial();
                string sourceFile = Path.Combine(materialsPath, MaterialNameTextBox.Text + ".json");
                if (File.Exists(sourceFile))
                {
                    File.Copy(sourceFile, saveDialog.FileName, true);
                    MessageBox.Show("Material guardado correctamente", "Material Editor");
                }
            }
        }

        private void LoadMaterial_Click(object sender, RoutedEventArgs e)
        {
            var openDialog = new Microsoft.Win32.OpenFileDialog
            {
                Filter = "Material Files (*.json)|*.json",
                InitialDirectory = Path.GetFullPath(materialsPath)
            };

            if (openDialog.ShowDialog() == true)
            {
                try
                {
                    string json = File.ReadAllText(openDialog.FileName);
                    var material = JsonConvert.DeserializeObject<dynamic>(json);

                    if (material != null)
                    {
                        MaterialNameTextBox.Text = material.name ?? "UnnamedMaterial";
                        
                        if (material.parameters?.BaseColor != null)
                        {
                            var baseColor = material.parameters.BaseColor.value;
                            BaseColorRSlider.Value = (double)baseColor[0];
                            BaseColorGSlider.Value = (double)baseColor[1];
                            BaseColorBSlider.Value = (double)baseColor[2];
                        }

                        if (material.parameters?.Metallic != null)
                        {
                            MetallicSlider.Value = (double)material.parameters.Metallic.value;
                        }

                        if (material.parameters?.Roughness != null)
                        {
                            RoughnessSlider.Value = (double)material.parameters.Roughness.value;
                        }

                        if (material.parameters?.Emissive != null)
                        {
                            var emissive = material.parameters.Emissive.value;
                            EmissiveRSlider.Value = (double)emissive[0];
                            EmissiveGSlider.Value = (double)emissive[1];
                            EmissiveBSlider.Value = (double)emissive[2];
                        }

                        currentMaterialPath = openDialog.FileName;
                        UpdateMaterial();
                    }
                }
                catch (Exception ex)
                {
                    MessageBox.Show($"Error cargando material: {ex.Message}", "Error", 
                                  MessageBoxButton.OK, MessageBoxImage.Error);
                }
            }
        }

        private void ResetMaterial_Click(object sender, RoutedEventArgs e)
        {
            BaseColorRSlider.Value = 1.0;
            BaseColorGSlider.Value = 0.0;
            BaseColorBSlider.Value = 0.0;
            MetallicSlider.Value = 0.0;
            RoughnessSlider.Value = 0.5;
            EmissiveRSlider.Value = 0.0;
            EmissiveGSlider.Value = 0.0;
            EmissiveBSlider.Value = 0.0;
            UpdateMaterial();
        }

        private void LoadDefaultMaterial()
        {
            string defaultMaterialPath = Path.Combine(materialsPath, "DefaultMaterial.json");
            if (File.Exists(defaultMaterialPath))
            {
                try
                {
                    string json = File.ReadAllText(defaultMaterialPath);
                    var material = JsonConvert.DeserializeObject<dynamic>(json);

                    if (material != null)
                    {
                        MaterialNameTextBox.Text = material.name ?? "DefaultMaterial";
                        
                        if (material.parameters?.BaseColor != null)
                        {
                            var baseColor = material.parameters.BaseColor.value;
                            BaseColorRSlider.Value = (double)baseColor[0];
                            BaseColorGSlider.Value = (double)baseColor[1];
                            BaseColorBSlider.Value = (double)baseColor[2];
                        }

                        if (material.parameters?.Metallic != null)
                        {
                            MetallicSlider.Value = (double)material.parameters.Metallic.value;
                        }

                        if (material.parameters?.Roughness != null)
                        {
                            RoughnessSlider.Value = (double)material.parameters.Roughness.value;
                        }

                        if (material.parameters?.Emissive != null)
                        {
                            var emissive = material.parameters.Emissive.value;
                            EmissiveRSlider.Value = (double)emissive[0];
                            EmissiveGSlider.Value = (double)emissive[1];
                            EmissiveBSlider.Value = (double)emissive[2];
                        }

                        currentMaterialPath = defaultMaterialPath;
                        UpdateMaterial();
                    }
                }
                catch (Exception)
                {
                    // Silenciar error si no se puede cargar material por defecto
                }
            }
        }

        private void OpenMainEditor_Click(object sender, RoutedEventArgs e)
        {
            // Buscar MainWindow y traerla al frente
            foreach (Window window in Application.Current.Windows)
            {
                if (window is MainWindow)
                {
                    window.Activate();
                    window.Focus();
                    return;
                }
            }
            
            // Si no existe, crear nueva
            MainWindow mainWindow = new MainWindow();
            mainWindow.Show();
        }
    }
}

