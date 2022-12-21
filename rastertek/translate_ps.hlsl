Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

cbuffer TransltaeBuffer : register( b0 )
{
	float translate_x;
	float translate_y;
}

float4 translate_ps( PS_INPUT input ) : SV_TARGET
{
	float2 tex = input.tex;

	tex.x += translate_x;
	tex.y += translate_y;

	float4 color = src_texture.Sample( sample_state, tex );
	return color;
}