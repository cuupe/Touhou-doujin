cbuffer MatrixBuffer : register(b0)
{
    float4x4 WorldViewProj;
};

struct VS_IN
{
    float3 pos : POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR;
};

struct VS_OUT
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;
    output.pos = mul(float4(input.pos, 1.0f), WorldViewProj);
    output.uv = input.uv;
    output.col = input.col;
    return output;
}