
Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

cbuffer SliceBuffer : register(b0)
{
    float4 resolution;
    float4 border;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 TexturePixelShader( PS_INPUT input ) : SV_TARGET
{
	float4 color = src_texture.Sample( sample_state, input.tex );
	return color;
}

float mapping(float value, float original_min, float original_max, float new_min, float new_max)
{
    return (value - original_min) / (original_max - original_min) * (new_max - new_min) + new_min;
}

float calculate(float coord, float2 src_uv, float2 dst_uv)
{
    if (coord < src_uv.x)
    {
        return mapping(coord, 0, src_uv.x, 0, dst_uv.x);
    }
    else if (coord < src_uv.y)
    {
        return mapping(coord, src_uv.x, src_uv.y, dst_uv.x, dst_uv.y);
    }
    else
    {
        return mapping(coord, src_uv.y, 1.0f, dst_uv.y, 1.0f);
    }
}

float2 make_uv(float2 border, float length)
{
    return float2(border.x / length, (length - border.y) / length);
}

float4 SlicePixelShader(PS_INPUT input) : SV_TARGET
{
    float2 quad_resolution = resolution.xy;
    float2 texture_resolution = resolution.zw;
    
    float2 src_uv_lr = make_uv(border.xz, quad_resolution.x);
    float2 dst_uv_lr = make_uv(border.xz, texture_resolution.x);
    
    float2 src_uv_tb = make_uv(border.yw, quad_resolution.y);
    float2 dst_uv_tb = make_uv(border.yw, texture_resolution.y);
    
    float2 uv;
    uv.x = calculate(input.tex.x, src_uv_lr, dst_uv_lr);
    uv.y = calculate(input.tex.y, src_uv_tb, dst_uv_tb);
    
    return src_texture.Sample(sample_state, uv);
}