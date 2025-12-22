cbuffer CBPerFrame : register(b1)
{
    matrix m_view;
    matrix m_proj;
    float4 camera_position;
    float4 ambient_color;
}

cbuffer CBPerObject : register(b2)
{
    matrix m_world;
    float4 mesh_color;
}

struct VS_IN
{
    float3 pos : POSITION;
    float3 norm : NORMAL;
    float2 uv : TEXCOORD;
    float4 col : COLOR;
};

			
struct VS_OUT
{
    float4 clip_pos : SV_POSITION;
    float3 w_pos : POSITION_W;
    float3 w_norm : NORMAL_W;
    float2 uv : TEXCOORD;
    float4 col : COLOR;
    float3 camera_pos : CAMERA_POS;
};

VS_OUT main(VS_IN input)
{
    VS_OUT output;

    float4 model_pos = float4(input.pos, 1.0f);
    float4 world_pos = mul(model_pos, m_world);
    float4 view_pos = mul(world_pos, m_view);
    float4 clip_pos = mul(view_pos, m_proj);

    output.clip_pos = clip_pos;
    output.w_pos = world_pos.xyz;
    
    output.w_norm = mul(input.norm, (float3x3) m_world);
    output.w_norm = normalize(output.w_norm);
    
    output.uv = input.uv;
    output.col = input.col * mesh_color;
    output.camera_pos = camera_position.xyz;
    
    return output;
}