Texture2D src_texture : register( t0 );
Texture2D reflect_texture : register( t1 );
SamplerState sample_state : register( s0 );

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflection_pos : TEXCOORD1;
};

float4 reflection_ps( PS_INPUT input ) : SV_TARGET
{
	float2 reflect_tex;
	reflect_tex.x = input.reflection_pos.x / input.reflection_pos.w / 2.0f + 0.5f;
	reflect_tex.y = -input.reflection_pos.y / input.reflection_pos.w / 2.0f + 0.5f;
	
	float4 src_color = src_texture.Sample( sample_state, input.tex );
	float4 reflect_color = reflect_texture.Sample( sample_state, reflect_tex );

	float4 color = lerp( src_color, reflect_color, 0.15f );

	return color;
}
