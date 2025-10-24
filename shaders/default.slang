struct VSInput {
    float3 pos : POSITION;
    float3 normal : NORMAL;
    float2 uv : TEXCOORD;
    float4 color : COLOR;
};

struct VSOutput {
    float4 position : SV_POSITION;
    float3 normal : NORMAL;
    float2 uv: TEXCOORD;
    float4 color : COLOR;
};

struct Transform{
    float4x4 model;
    float4x4 view;
    float4x4 projection;
};

[[vk::binding(0, 0)]] ConstantBuffer<Transform> transform;
[[vk::binding(1, 0)]] ConstantBuffer<Sampler2D> sampler;

[shader("vertex")]
VSOutput vertexMain(VSInput input)
{
    VSOutput output;
    output.position = mul(transform.projection,
                        mul(transform.view,
                        mul(transform.model, 
                        float4(input.pos, 1.0))));

    output.normal = input.normal;
    output.uv = input.uv;
    output.color = input.color;

    return output;
}

[shader("fragment")]
float4 fragmentMain(VSOutput input) 
{
    float3 lightDir = normalize(float3(0.5, 1.0, 0.5));
    float3 normal = normalize(input.normal);
    float diffuse = max(dot(normal, lightDir), 0.0);
    float ambient = 0.3;
    
    float3 lighting = float3(ambient + diffuse * 0.7);
    
    // float4 texColor = sampler.Sample(input.uv);
    // return float4(lighting, 1.0) * texColor * input.color;
    
    return float4(lighting * input.color.rgb, input.color.a);
}
