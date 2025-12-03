// Pixel shader para escalado de alta calidad del render target
// Escala desde 1920x1080 al tamaño del viewport con filtrado bilinear

Texture2D sourceTexture : register(t0);
SamplerState linearSampler : register(s0);

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

float4 main(PSInput input) : SV_TARGET
{
    // Usar muestreo bilinear para escalado suave
    return sourceTexture.Sample(linearSampler, input.uv);
}

