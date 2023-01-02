Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

cbuffer LightBuffer
{
	float4 ambient;
	float4 diffuse;
	float3 light_direction;
	float padding;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float clip : SV_ClipDistance0;
};

float4 refract_ps( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( sample_state, input.tex );
	float4 color = ambient;
	float3 direction = -light_direction;
	float light_intensity = saturate( dot( input.normal, direction ) );

	if ( light_intensity > 0.0f )
	{
		color += diffuse * light_intensity;
	}

	color = saturate( color );

	color *= src_color;

	return color;

}