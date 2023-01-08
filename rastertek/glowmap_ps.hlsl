Texture2D src_texture : register( t0 );
Texture2D glowmap_texture : register( t1 );

SamplerState wrap_state : register( s0 );

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 glowmap_ps( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( wrap_state, input.tex );
	float4 glowmap = glowmap_texture.Sample( wrap_state, input.tex );

	float4 color;
	if ( ( 0.0f == glowmap.b ) && ( 0.0f == glowmap.g ) && ( 0.0f == glowmap.r ) )
	{
		color = float4( 0.0f, 0.0f, 0.0f, 1.0f );
	}
	else
	{
		color = src_color;
	}

	return color;
}