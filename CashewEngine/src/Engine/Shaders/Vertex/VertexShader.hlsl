struct Output
{
    float4 outColor : COLOR;
    float4 outPosition : SV_Position;
};

Output main(float3 pos: POSITION, float3 incolor : COLOR)
{
    Output vertexOut;
    
    vertexOut.outPosition = float4(pos, 1.0f);
    vertexOut.outColor = float4(incolor, 1.0f);
    
    return vertexOut;
}