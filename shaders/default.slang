struct VSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float4 color : COLOR;
};

struct Transform{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

uniform Transform transform;

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    output.position = mul(transform.projection,
                        mul(transform.view,
                        mul(transform.model, 
                        float4(input.pos, 1.0))));

    output.color = input.color;

    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) 
{
    return input.color;
}
