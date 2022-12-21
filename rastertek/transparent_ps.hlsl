Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

cbuffer TransparentBuffer : register( b0 )
{
	float blend_amount;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 transparent_ps( PS_INPUT input ) : SV_TARGET
{
	float4 color = src_texture.Sample( sample_state, input.tex );

	color.a = blend_amount;

	return color;
}