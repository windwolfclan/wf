Texture2D src_texture : register( t0 );
Texture2D dst_texture : register( t1 );
Texture2D alphamap : register( t2 );

SamplerState sample_state : register( s0 );

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 alphamap_ps( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( sample_state, input.tex );
	float4 dst_color = dst_texture.Sample( sample_state, input.tex );
	float4 alpha = alphamap.Sample( sample_state, input.tex );

	float4 color = ( src_color * alpha ) + ( dst_color * ( 1.0f - alpha ) );
	color = saturate( color );

	return color;
}