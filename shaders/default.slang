struct VSInput {
    // float3 position : POSITION;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR0;
};

[shader("vertex")]
VSOutput vertexMain(uint vertex_id : SV_VertexID) 
{
    VSOutput output;

    float2 positions[] = {
        float2(0.0, -0.5),
        float2(-0.5, 0.5),
        float2(0.5, 0.5),
    };
    output.position = float4(positions[vertex_id], 0.0, 1.0);

    float3 colors[] = {
        float3(1.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        float3(0.0, 0.0, 1.0)
    };
    output.color = float4(colors[vertex_id], 1.0);

    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) 
{
    return input.color;
}
