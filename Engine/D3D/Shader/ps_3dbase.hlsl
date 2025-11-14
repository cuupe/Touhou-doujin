Texture2D texDiffuse : register(t0);
SamplerState samLinear : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR;
};

float4 main(PS_IN i) : SV_Target
{
    float4 tex = texDiffuse.Sample(samLinear, i.uv);
    return tex * i.col;
}