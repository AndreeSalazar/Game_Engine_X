// Vertex shader para escalado de alta calidad
// Renderiza un quad de pantalla completa

struct VSInput
{
    float2 position : POSITION;
    float2 uv : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

PSInput main(VSInput input)
{
    PSInput output;
    // Pasar las coordenadas directamente (ya están en espacio de pantalla)
    output.position = float4(input.position.x, input.position.y, 0.0, 1.0);
    output.uv = input.uv;
    return output;
}

