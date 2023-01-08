Texture2D src_texture : register( t0 );
Texture2D glow_texture : register( t1 );

SamplerState sample_state : register( s0 );

cbuffer GlowBuffer : register( b0 )
{
	float glow_strength;
	float3 padding;
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 glow_ps( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( sample_state, input.tex );
	float4 glow = glow_texture.Sample( sample_state, input.tex );

	float4 color = saturate( src_color + ( glow_strength * glow ) );
	return color;
}