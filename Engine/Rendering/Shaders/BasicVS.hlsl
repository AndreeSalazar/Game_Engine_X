// Vertex Shader para DirectX 12 - Estilo Vulkan Cube
// Optimizado para RTX 3060 12GB

cbuffer MVPBuffer : register(b0) {
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

struct VertexInput {
    float3 position : POSITION;
    float3 color : COLOR;
};

struct VertexOutput {
    float4 position : SV_POSITION;
    float3 color : COLOR;
};

VertexOutput main(VertexInput input) {
    VertexOutput output;
    
    // Aplicar transformaciones MVP
    // En DirectX con matrices transpuestas (row-major), multiplicamos paso a paso:
    // worldPos * model * view * projection
    float4 worldPos = float4(input.position, 1.0f);
    
    // Multiplicar paso a paso para asegurar el orden correcto
    float4 worldPosTransformed = mul(worldPos, model);
    float4 viewPos = mul(worldPosTransformed, view);
    float4 clipPos = mul(viewPos, projection);
    
    output.position = clipPos;
    output.color = input.color;
    
    return output;
}

