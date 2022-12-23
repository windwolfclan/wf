#define LIGHT_COUNT 4

Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

cbuffer LightColorBuffer : register( b0 )
{
	float4 diffuse[ LIGHT_COUNT ];
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 light_pos1 : TEXCOORD1;
	float3 light_pos2 : TEXCOORD2;
	float3 light_pos3 : TEXCOORD3;
	float3 light_pos4 : TEXCOORD4;
};

float4 multi_light_ps( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( sample_state, input.tex );

	float light_intensity1 = saturate( dot( input.normal, input.light_pos1 ) );
	float light_intensity2 = saturate( dot( input.normal, input.light_pos2 ) );
	float light_intensity3 = saturate( dot( input.normal, input.light_pos3 ) );
	float light_intensity4 = saturate( dot( input.normal, input.light_pos4 ) );

	float4 light_color1 = diffuse[ 0 ] * light_intensity1;
	float4 light_color2 = diffuse[ 1 ] * light_intensity2;
	float4 light_color3 = diffuse[ 2 ] * light_intensity3;
	float4 light_color4 = diffuse[ 3 ] * light_intensity4;

	float4 color = saturate( light_color1 + light_color2 + light_color3 + light_color4 ) * src_color;
	return color;
}