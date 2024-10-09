struct Output
{
    float4 outColor : COLOR;
    float4 outPosition : SV_Position;
};

struct Rotation
{
    matrix transform;
};

ConstantBuffer<Rotation> rot : register(b0);

Output main(float3 inpos: POSITION)
{
    Output vertexOut;
    
    vertexOut.outPosition = mul(float4(inpos, 1.0f), rot.transform);
    
    return vertexOut;
}