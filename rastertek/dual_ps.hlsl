Texture2D src_texture[ 2 ] : register( t0 );
SamplerState sample_state : register( s0 );

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 dual_ps( PS_INPUT input ) : SV_TARGET
{
	float4 color1 = src_texture[ 0 ].Sample( sample_state, input.tex );
	float4 color2 = src_texture[ 1 ].Sample( sample_state, input.tex );

	float4 color = color1 * color2 * 2.0f;
	color = saturate( color );

	return color;
}