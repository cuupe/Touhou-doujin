cbuffer MatrixBuffer : register(b0)
{
    float4x4 world_view_proj;
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
    float4 local_pos = float4(input.pos, 1.0f);
    output.pos = mul(local_pos, world_view_proj);
    output.uv = input.uv;
    output.col = input.col;

    return output;
}