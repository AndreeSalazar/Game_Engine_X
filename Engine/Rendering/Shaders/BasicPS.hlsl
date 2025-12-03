// Pixel Shader mejorado para DirectX 12
// Optimizado para RTX 3060 12GB

struct PixelInput {
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

float4 main(PixelInput input) : SV_TARGET {
    // Mejorar los colores con saturación y brillo
    float3 finalColor = saturate(input.color);
    
    // Aumentar ligeramente el brillo para colores más vibrantes
    finalColor = pow(finalColor, 0.9f); // Gamma correction ligera
    
    return float4(finalColor, 1.0f);
}

