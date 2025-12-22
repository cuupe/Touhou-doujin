Texture2D tex : register(t0);
SamplerState samp : register(s0);

struct PS_IN
{
    float4 pos : SV_POSITION;
    float2 uv : TEXCOORD0;
    float4 col : COLOR;
};

float4 main(PS_IN input) : SV_Target
{
    return tex.Sample(samp, input.uv) * input.col;
}