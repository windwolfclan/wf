
Texture2D src_texture : register( t0 );
SamplerState wrap_sample : register( s0 );


struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PS_OUTPUT
{
	float4 color : SV_TARGET0;
	float4 normal : SV_TARGET1;
};

PS_OUTPUT deffered_ps( PS_INPUT input )
{
	PS_OUTPUT output = (PS_OUTPUT)0;

	output.color = src_texture.Sample( wrap_sample, input.tex );
	output.normal = float4( input.normal, 1.0f );

	return output;
}