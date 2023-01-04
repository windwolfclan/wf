
SamplerState sample_state : register( s0 );

Texture2D src_texture : register( t0 );
Texture2D normal_texture : register( t1 );
Texture2D refract_texture : register( t2 );

cbuffer GlassBuffer : register( b0 )
{
	float refract_scale;
	float3 padding;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 refract_pos : TEXCOORD1;
};

float4 glass_ps( PS_INPUT input ) : SV_TARGET
{
	float2 refract_tex;
	refract_tex.x = input.refract_pos.x / input.refract_pos.w / 2.0f + 0.5f;
	refract_tex.y = -input.refract_pos.y / input.refract_pos.w / 2.0f + 0.5f;

	float4 normal_map = normal_texture.Sample( sample_state, input.tex );
	float3 normal = ( normal_map.xyz * 2.0f ) - 1.0f;

	refract_tex += normal.xy * refract_scale;

	float4 refract_color = refract_texture.Sample( sample_state, refract_tex );

	float4 src_color = src_texture.Sample( sample_state, input.tex );

	float4 color = lerp( refract_color, src_color, 0.5f );

	return color;
}
