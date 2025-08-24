struct VSInput {
    float3 position : POSITION;
};

struct VSOutput {
    float4 position : SV_POSITION;
};

[shader("vertex")]
VSOutput vertexMain(VSInput input, uint vertex_id : SV_VertexID)
{
    VSOutput output;
    output.position = float4(input.position, 1.0);

    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) 
{
    return (input.position);
}
