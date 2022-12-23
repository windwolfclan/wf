Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

cbuffer FadeBuffer : register( b0 )
{
	float fade_amount;
	float3 padding;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 fade_ps( PS_INPUT input ) : SV_TARGET
{
	float4 color = src_texture.Sample( sample_state, input.tex );
	return color * fade_amount;
}