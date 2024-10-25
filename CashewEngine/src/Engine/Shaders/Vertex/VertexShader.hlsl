struct Output
{
    float4 outColor : COLOR;
    float4 outPosition : SV_Position;
};

struct mvp
{
    matrix transform;
};

ConstantBuffer<mvp> m : register(b0);


Output main(float3 inpos: POSITION0, float4 incolor: COLOR0, uint vertexID : SV_VertexID)
{
    Output vertexOut;
    
    vertexOut.outPosition = mul(float4(inpos, 1.0f), m.transform);
    vertexOut.outColor = incolor;
    
    return vertexOut;
}