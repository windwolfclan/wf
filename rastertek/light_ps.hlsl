Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

cbuffer LightBuffer
{
	float4 diffuse;
	float3 direction;
	float padding;
};


struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 LightPixelShader( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( sample_state, input.tex );

	float3 light_direction = -direction;
	float intensity = saturate( dot( input.normal, light_direction ) );

	float4 color = saturate( diffuse * intensity );

	color *= src_color;

	return color;
}