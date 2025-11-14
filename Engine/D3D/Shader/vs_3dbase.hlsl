cbuffer MatrixBuffer : register(b0)
{
    matrix mWorld;
    matrix mView;
    matrix mProjection;
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

    float4 model_pos = float4(input.pos, 1.0f);

    float4 world_pos = mul(model_pos, mWorld);
    float4 view_pos = mul(world_pos, mView);
    float4 clip_pos = mul(view_pos, mProjection);

    output.pos = clip_pos;

    output.uv = input.uv;
    output.col = input.col;

    return output;
}