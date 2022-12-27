Texture2D fire_texture : register( t0 );
Texture2D noise_texture : register( t1 );
Texture2D alpha_texture : register( t2 );

SamplerState sample_state1 : register( s0 );
SamplerState sample_state2 : register( s1 );

cbuffer DistortionBuffer : register( b0 )
{
	float2 distortion1;
	float2 distortion2;
	float2 distortion3;
	float distortion_scale;
	float distortion_bias;
};


struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float2 coord1 : TEXCOORD1;
	float2 coord2 : TEXCOORD2;
	float2 coord3 : TEXCOORD3;
};


float4 fire_ps( PS_INPUT input ) : SV_TARGET
{
	float4 noise1 = noise_texture.Sample( sample_state1, input.coord1 );
	float4 noise2 = noise_texture.Sample( sample_state1, input.coord2 );
	float4 noise3 = noise_texture.Sample( sample_state1, input.coord3 );

	noise1 = ( noise1 - 0.5f ) * 2.0f;
	noise2 = ( noise1 - 0.5f ) * 2.0f;
	noise3 = ( noise1 - 0.5f ) * 2.0f;

	noise1.xy = noise1.xy * distortion1;
	noise2.xy = noise2.xy * distortion2;
	noise3.xy = noise3.xy * distortion3;

	float4 noise = noise1 + noise2 + noise3;
	
	float perturb = ( ( 1.0f - input.tex.y ) * distortion_scale ) + distortion_bias;
	
	float2 noise_tex = ( noise.xy * perturb ) + input.tex.xy;

	float4 fire_color = fire_texture.Sample( sample_state2, noise_tex );
	float4 alpha_color = alpha_texture.Sample( sample_state2, noise_tex );

	fire_color.a = alpha_color;

	return fire_color;
}