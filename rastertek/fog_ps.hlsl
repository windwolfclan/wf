Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float fog : FOG;
};

float4 fog_ps( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( sample_state, input.tex );

	float4 fog_color = float4( 0.5f, 0.5f, 0.5f, 1.0f );

	float4 color = input.fog * src_color + ( 1.0f - input.fog ) * fog_color;

	return color;
}