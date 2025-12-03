using System;
using System.Drawing;
using System.Runtime.InteropServices;
using System.Windows;
using System.Windows.Interop;

namespace DirectX12Editor
{
    /// <summary>
    /// Sistema de escalado automático del viewport similar a Unreal Engine 5
    /// Ajusta automáticamente el viewport según la resolución disponible y el DPI del sistema
    /// </summary>
    public class ViewportAutoScaler
    {
        [DllImport("user32.dll")]
        private static extern IntPtr GetDC(IntPtr hWnd);

        [DllImport("user32.dll")]
        private static extern int ReleaseDC(IntPtr hWnd, IntPtr hDC);

        [DllImport("gdi32.dll")]
        private static extern int GetDeviceCaps(IntPtr hdc, int nIndex);

        private const int LOGPIXELSX = 88;
        private const int LOGPIXELSY = 90;

        /// <summary>
        /// Obtiene el DPI del sistema
        /// </summary>
        public static double GetSystemDPI()
        {
            IntPtr hdc = GetDC(IntPtr.Zero);
            if (hdc != IntPtr.Zero)
            {
                int dpiX = GetDeviceCaps(hdc, LOGPIXELSX);
                ReleaseDC(IntPtr.Zero, hdc);
                return dpiX / 96.0; // Normalizar a escala de 96 DPI
            }
            return 1.0;
        }

        /// <summary>
        /// Calcula el tamaño del viewport ajustado automáticamente según la resolución disponible
        /// </summary>
        public static ViewportSize CalculateAutoScaledViewport(
            double availableWidth,
            double availableHeight,
            double? targetAspectRatio = null,
            double minScale = 0.5,
            double maxScale = 2.0)
        {
            if (availableWidth <= 0 || availableHeight <= 0)
            {
                return new ViewportSize { Width = 800, Height = 600, Scale = 1.0 };
            }

            // Obtener DPI del sistema
            double dpiScale = GetSystemDPI();
            
            // Aplicar escala de DPI
            double scaledWidth = availableWidth * dpiScale;
            double scaledHeight = availableHeight * dpiScale;

            // Calcular escala basada en resolución de referencia (1920x1080)
            const double referenceWidth = 1920.0;
            const double referenceHeight = 1080.0;
            
            double scaleX = scaledWidth / referenceWidth;
            double scaleY = scaledHeight / referenceHeight;
            
            // Usar la escala más pequeña para mantener el aspect ratio
            double scale = Math.Min(scaleX, scaleY);
            
            // Limitar la escala entre min y max
            scale = Math.Max(minScale, Math.Min(maxScale, scale));

            // Si se especifica un aspect ratio objetivo, ajustar el tamaño
            if (targetAspectRatio.HasValue)
            {
                double targetRatio = targetAspectRatio.Value;
                double currentRatio = scaledWidth / scaledHeight;

                if (currentRatio > targetRatio)
                {
                    // Demasiado ancho, reducir el ancho
                    scaledWidth = scaledHeight * targetRatio;
                }
                else
                {
                    // Demasiado alto, reducir la altura
                    scaledHeight = scaledWidth / targetRatio;
                }
            }

            // Aplicar la escala calculada
            int viewportWidth = (int)(scaledWidth * scale);
            int viewportHeight = (int)(scaledHeight * scale);

            // Asegurar tamaño mínimo
            if (viewportWidth < 320) viewportWidth = 320;
            if (viewportHeight < 240) viewportHeight = 240;

            return new ViewportSize
            {
                Width = viewportWidth,
                Height = viewportHeight,
                Scale = scale,
                DpiScale = dpiScale
            };
        }

        /// <summary>
        /// Calcula el tamaño del viewport con límites seguros (safe area) como en Unreal Engine 5
        /// </summary>
        public static ViewportSize CalculateSafeAreaViewport(
            double availableWidth,
            double availableHeight,
            double safeAreaMargin = 0.05) // 5% de margen por defecto
        {
            if (availableWidth <= 0 || availableHeight <= 0)
            {
                return new ViewportSize { Width = 800, Height = 600, Scale = 1.0 };
            }

            // Calcular área segura (dejar margen en los bordes)
            double safeWidth = availableWidth * (1.0 - safeAreaMargin * 2);
            double safeHeight = availableHeight * (1.0 - safeAreaMargin * 2);

            // Asegurar tamaño mínimo
            if (safeWidth < 320) safeWidth = 320;
            if (safeHeight < 240) safeHeight = 240;

            return new ViewportSize
            {
                Width = (int)safeWidth,
                Height = (int)safeHeight,
                Scale = 1.0,
                DpiScale = GetSystemDPI()
            };
        }

        /// <summary>
        /// Obtiene la resolución del monitor principal
        /// </summary>
        public static System.Drawing.Size GetPrimaryMonitorResolution()
        {
            return new System.Drawing.Size(
                (int)SystemParameters.PrimaryScreenWidth,
                (int)SystemParameters.PrimaryScreenHeight
            );
        }

        /// <summary>
        /// Calcula el tamaño del viewport con escalado inteligente estilo Unreal Engine 5
        /// Considera DPI, resolución disponible y mantiene calidad visual
        /// </summary>
        public static ViewportSize CalculateIntelligentViewport(
            double availableWidth,
            double availableHeight,
            bool maintainAspectRatio = false,
            double? preferredAspectRatio = null)
        {
            if (availableWidth <= 0 || availableHeight <= 0)
            {
                return new ViewportSize { Width = 800, Height = 600, Scale = 1.0 };
            }

            // Obtener DPI del sistema
            double dpiScale = GetSystemDPI();
            
            // Resolución base de referencia (Full HD) - declarar una sola vez
            const double baseWidth = 1920.0;
            const double baseHeight = 1080.0;
            
            // Declarar variables fuera del bloque if para evitar redeclaración
            int viewportWidth;
            int viewportHeight;
            double scale;
            double scaleX;
            double scaleY;
            
            // Si no se requiere mantener aspect ratio, usar TODO el espacio disponible
            if (!maintainAspectRatio)
            {
                // Usar directamente el tamaño disponible para llenar completamente el espacio
                viewportWidth = (int)availableWidth;
                viewportHeight = (int)availableHeight;
                
                // Asegurar tamaño mínimo (320x240)
                if (viewportWidth < 320) viewportWidth = 320;
                if (viewportHeight < 240) viewportHeight = 240;
                
                // Calcular escala basada en resolución de referencia
                scaleX = viewportWidth / baseWidth;
                scaleY = viewportHeight / baseHeight;
                scale = Math.Min(scaleX, scaleY);
                
                return new ViewportSize
                {
                    Width = viewportWidth,
                    Height = viewportHeight,
                    Scale = scale,
                    DpiScale = dpiScale
                };
            }
            
            // Si se requiere mantener aspect ratio, calcular con restricciones
            // Calcular resolución efectiva considerando DPI
            double effectiveWidth = availableWidth * dpiScale;
            double effectiveHeight = availableHeight * dpiScale;
            
            // Calcular escala basada en resolución disponible
            scaleX = effectiveWidth / baseWidth;
            scaleY = effectiveHeight / baseHeight;
            
            // Usar la escala más pequeña para mantener proporciones
            scale = Math.Min(scaleX, scaleY);
            
            // Limitar escala entre 0.25x y 4.0x (rango razonable)
            scale = Math.Max(0.25, Math.Min(4.0, scale));
            
            // Calcular tamaño del viewport
            viewportWidth = (int)(effectiveWidth * scale);
            viewportHeight = (int)(effectiveHeight * scale);
            
            // Si se requiere mantener aspect ratio específico
            if (preferredAspectRatio.HasValue)
            {
                double targetRatio = preferredAspectRatio.Value;
                double currentRatio = (double)viewportWidth / viewportHeight;
                
                if (currentRatio > targetRatio)
                {
                    viewportWidth = (int)(viewportHeight * targetRatio);
                }
                else
                {
                    viewportHeight = (int)(viewportWidth / targetRatio);
                }
            }
            
            // Asegurar tamaño mínimo (320x240)
            if (viewportWidth < 320) viewportWidth = 320;
            if (viewportHeight < 240) viewportHeight = 240;
            
            // Asegurar que el tamaño no exceda el disponible
            if (viewportWidth > availableWidth) viewportWidth = (int)availableWidth;
            if (viewportHeight > availableHeight) viewportHeight = (int)availableHeight;
            
            return new ViewportSize
            {
                Width = viewportWidth,
                Height = viewportHeight,
                Scale = scale,
                DpiScale = dpiScale
            };
        }

        /// <summary>
        /// Calcula la resolución óptima de renderizado dentro del rango 800x600 a 1920x1080
        /// basándose en el espacio disponible del editor
        /// </summary>
        public static RenderResolution CalculateOptimalRenderResolution(
        double availableWidth,
        double availableHeight,
        int minWidth = 800,
        int minHeight = 600,
        int maxWidth = 1920,
        int maxHeight = 1080)
    {
        if (availableWidth <= 0 || availableHeight <= 0)
        {
            return new RenderResolution { Width = 800, Height = 600, Quality = 0.5 };
        }

        // Obtener DPI del sistema
        double dpiScale = GetSystemDPI();
        
        // Resoluciones estándar dentro del rango permitido
        var standardResolutions = new[]
        {
            new { Width = 1920, Height = 1080, Quality = 1.0 },  // Full HD (máxima calidad)
            new { Width = 1600, Height = 900, Quality = 0.83 },
            new { Width = 1366, Height = 768, Quality = 0.71 },
            new { Width = 1280, Height = 720, Quality = 0.67 },  // HD
            new { Width = 1024, Height = 768, Quality = 0.53 },
            new { Width = 800, Height = 600, Quality = 0.42 }   // Mínima calidad
        };

        // Calcular el aspect ratio del espacio disponible
        double availableAspectRatio = availableWidth / availableHeight;
        
        // Encontrar la resolución estándar más cercana que:
        // 1. Quepa dentro del espacio disponible
        // 2. Tenga un aspect ratio similar
        // 3. Maximice la calidad dentro del rango permitido
        
        RenderResolution bestResolution = new RenderResolution { Width = 800, Height = 600, Quality = 0.42 };
        double bestScore = 0;
        
        foreach (var res in standardResolutions)
        {
            // Verificar que la resolución esté dentro del rango permitido
            if (res.Width < minWidth || res.Height < minHeight ||
                res.Width > maxWidth || res.Height > maxHeight)
            {
                continue;
            }
            
            // Verificar que la resolución quepa en el espacio disponible
            if (res.Width > availableWidth || res.Height > availableHeight)
            {
                continue;
            }
            
            // Calcular el aspect ratio de esta resolución
            double resAspectRatio = (double)res.Width / res.Height;
            
            // Calcular un score basado en:
            // - Calidad (mayor es mejor)
            // - Aspect ratio match (más cercano es mejor)
            // - Uso del espacio disponible (más espacio usado es mejor)
            double aspectRatioMatch = 1.0 - Math.Abs(resAspectRatio - availableAspectRatio);
            double spaceUsage = (res.Width * res.Height) / (availableWidth * availableHeight);
            
            // Score combinado (priorizar calidad, luego aspect ratio, luego uso del espacio)
            double score = (res.Quality * 0.5) + (aspectRatioMatch * 0.3) + (spaceUsage * 0.2);
            
            if (score > bestScore)
            {
                bestScore = score;
                bestResolution = new RenderResolution
                {
                    Width = res.Width,
                    Height = res.Height,
                    Quality = res.Quality
                };
            }
        }
        
        // Si no encontramos una resolución estándar adecuada, calcular una personalizada
        if (bestScore == 0)
        {
            // Calcular resolución personalizada dentro del rango
            int customWidth = Math.Max(minWidth, Math.Min(maxWidth, (int)availableWidth));
            int customHeight = Math.Max(minHeight, Math.Min(maxHeight, (int)availableHeight));
            
            // Ajustar para mantener aspect ratio si es necesario
            double customAspectRatio = (double)customWidth / customHeight;
            if (Math.Abs(customAspectRatio - availableAspectRatio) > 0.1)
            {
                // Ajustar para mantener el aspect ratio del espacio disponible
                if (customAspectRatio > availableAspectRatio)
                {
                    customWidth = (int)(customHeight * availableAspectRatio);
                }
                else
                {
                    customHeight = (int)(customWidth / availableAspectRatio);
                }
                
                // Asegurar que esté dentro del rango
                customWidth = Math.Max(minWidth, Math.Min(maxWidth, customWidth));
                customHeight = Math.Max(minHeight, Math.Min(maxHeight, customHeight));
            }
            
            // Calcular calidad relativa (0.0 a 1.0)
            double quality = ((customWidth * customHeight) - (minWidth * minHeight)) / 
                            ((double)(maxWidth * maxHeight) - (minWidth * minHeight));
            
            bestResolution = new RenderResolution
            {
                Width = customWidth,
                Height = customHeight,
                Quality = quality
            };
        }
        
        return bestResolution;
        }
    }

    /// <summary>
    /// Estructura que contiene el tamaño calculado del viewport y su escala
    /// </summary>
    public struct ViewportSize
    {
        public int Width { get; set; }
        public int Height { get; set; }
        public double Scale { get; set; }
        public double DpiScale { get; set; }

        public override string ToString()
        {
            return $"Viewport: {Width}x{Height} (Scale: {Scale:F2}x, DPI: {DpiScale:F2}x)";
        }
    }

    /// <summary>
    /// Estructura que contiene la resolución óptima de renderizado calculada
    /// </summary>
    public struct RenderResolution
    {
        public int Width { get; set; }
        public int Height { get; set; }
        public double Quality { get; set; } // 0.0 a 1.0 (calidad relativa)

        public override string ToString()
        {
            return $"Render: {Width}x{Height} (Quality: {Quality:P0})";
        }
    }
}

