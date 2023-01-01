SamplerState sample_state : register( s0 );

Texture2D reflect_texture : register( t0 );
Texture2D refract_texture : register( t1 );
Texture2D normal_texture : register( t2 );

cbuffer WaterBuffer : register( b0 )
{
	float water_translation;
	float reflect_refract_scale;
	float2 padding;
}


struct PS_INPUT
{
	float4 pos : sv_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflect_pos : TEXCOORD1;
	float4 refract_pos : TEXCOORD2;
};

float4 water_ps( PS_INPUT input ) : SV_TARGET
{
	float2 tex = input.tex;
	tex.y += water_translation;

	float4 normal_map = normal_texture.Sample( sample_state, tex );
	float3 normal = ( normal_map.xyz * 2.0f ) - 1.0f;


	float2 reflect_tex;
	reflect_tex.x = input.reflect_pos.x / input.reflect_pos.w / 2.0f + 0.5f;
	reflect_tex.y = -input.reflect_pos.y / input.reflect_pos.w / 2.0f + 0.5f;
	reflect_tex += normal.xy * reflect_refract_scale;

	float4 reflect_color = reflect_texture.Sample( sample_state, reflect_tex );

	float2 refract_tex;
	refract_tex.x = input.refract_pos.x / input.refract_pos.w / 2.0f + 0.5f;
	refract_tex.y = -input.refract_pos.y / input.refract_pos.w / 2.0f + 0.5f;
	refract_tex += normal.xy * reflect_refract_scale;
	
	float4 refract_color = refract_texture.Sample( sample_state, refract_tex );

	float4 color = lerp( reflect_color, refract_color, 0.6f );

	return color;
}