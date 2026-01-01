#define MAX_LIGHTS_SIZE 16
Texture2D tex : register(t0);
Texture2D dep_texture : register(t1);
SamplerState samp : register(s0);

static const float near_plane = 0.1f;
static const float far_plane = 100.0f;

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
    float4 mix;
}

struct Light
{
    float4 color;
    float3 position;
    uint light_type;
    float3 direction;
    float constant_attenuation;
    float linear_attenuation;
    float quadratic_attenuation;
    float inner_cos_angle;
    float outer_cos_angle;
};

cbuffer CBLight : register(b3)
{
    Light lights[MAX_LIGHTS_SIZE];
    float4 light_count_padding;
};

cbuffer CBFog : register(b4)
{
    float fog_start;
    float3 fog_color;
    float fog_end;
    float3 fog_enable;
}

struct PS_IN
{
    float4 clip_pos : SV_POSITION;
    float3 w_pos : POSITION_W;
    float3 w_norm : NORMAL_W;
    float2 uv : TEXCOORD;
    float4 col : COLOR;
    float3 camera_pos : CAMERA_POS;
};

float linearize_depth(float depth)
{
    return (2.0 * near_plane) / (far_plane + near_plane - depth * (far_plane - near_plane));
}

float4 cal_light(PS_IN input)
{
    uint light_count = (uint) light_count_padding.x;
    if (light_count == 0)
    {
        return ambient_color;
    }

    float4 total_light = ambient_color;
    float3 w_norm = normalize(input.w_norm);

    for (uint i = 0; i < light_count; ++i)
    {
        Light current_light = lights[i];
        float3 light_direction = float3(0, 0, 0);
        float attenuation = 1.0f;
        float diffuse_factor = 0.0f;

        if (current_light.light_type == 0) // å¹³è¡Œå…?
        {
            light_direction = -normalize(current_light.direction);
            diffuse_factor = saturate(dot(w_norm, light_direction));
        }
        else if (current_light.light_type == 1) // ç‚¹å…‰æº?
        {
            float3 light_vec = current_light.position - input.w_pos;
            float dist = length(light_vec);
            light_direction = normalize(light_vec);

            attenuation = 1.0f / (current_light.constant_attenuation +
                                  current_light.linear_attenuation * dist +
                                  current_light.quadratic_attenuation * dist * dist);
            attenuation = max(attenuation, 0.1f); // é™åˆ¶æœ€å°è¡°å‡?
            diffuse_factor = saturate(dot(w_norm, light_direction));
        }
        else if (current_light.light_type == 2) // èšå…‰ç?
        {
            float3 light_vec = current_light.position - input.w_pos;
            float dist = length(light_vec);
            light_direction = normalize(light_vec);

            attenuation = 1.0f / (current_light.constant_attenuation +
                                  current_light.linear_attenuation * dist +
                                  current_light.quadratic_attenuation * dist * dist);
            attenuation = max(attenuation, 0.1f);

            float3 light_to_pixel_dir = -light_direction;
            float spot_factor = dot(light_to_pixel_dir, normalize(current_light.direction));

            if (current_light.inner_cos_angle > current_light.outer_cos_angle)
            {
                if (spot_factor > current_light.outer_cos_angle)
                {
                    float cone_factor = saturate((spot_factor - current_light.outer_cos_angle) /
                                                 (current_light.inner_cos_angle - current_light.outer_cos_angle));
                    attenuation *= cone_factor;
                }
                else
                {
                    attenuation = 0.0f;
                }
            }
            else
            {
                attenuation = 0.0f;
            }

            diffuse_factor = saturate(dot(w_norm, light_direction));
        }
        else
        {
            return ambient_color;
        }

        float4 light_contribution = current_light.color * diffuse_factor * attenuation;
        light_contribution = min(light_contribution, float4(1.0f, 1.0f, 1.0f, 1.0f));
        total_light += light_contribution;
    }

    total_light = min(total_light, float4(1.0f, 1.0f, 1.0f, 1.0f));
    return total_light;
}

float cal_fog(PS_IN input)
{
    float3 world_pos = input.w_pos;
    float3 camera_pos = input.camera_pos;
    float fog_distance = length(camera_pos - world_pos);

    if (fog_enable.x < 0.5f)
    {
        return 1.0f;
    }
    float fog_factor = 1.0f;

    if (fog_enable.y < 1.5f)
    {
        fog_factor = saturate((fog_end - fog_distance) / (fog_end - fog_start));
    }
    else if (fog_enable.y < 2.5f)
    {
        float density = fog_enable.z;
        if (density < 0.01f)
            density = 0.02f;
        fog_factor = exp(-density * fog_distance);
    }
    else if (fog_enable.y < 3.5f)
    {

        float density = fog_enable.z;
        if (density < 0.001f)
            density = 0.005f;
        fog_factor = exp(-density * fog_distance * fog_distance);
    }
    else
    {
        fog_factor = 1.0f;
    }

    return fog_factor;
}

float4 main(PS_IN input) : SV_Target
{
    float4 color_A = tex.Sample(samp, input.uv) * input.col * mesh_color;
    float4 base_color;

    if (mix.x > 0.001f)
    {
        float4 color_B = dep_texture.Sample(samp, input.uv) * input.col * mesh_color;
        base_color = lerp(color_A, color_B, mix.x);
    }
    else
    {
        base_color = color_A;
    }

    float4 total_light = cal_light(input);
    float4 lit_color = base_color * total_light;
    
    float fog_factor = cal_fog(input);
    if (fog_enable.x > 0.5f)
    {
        return lerp(float4(fog_color.rgb, 1.0f), lit_color, fog_factor);
    }

    return lit_color;
}
