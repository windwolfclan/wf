Texture2D src_texture : register( t0 );
SamplerState sample_state : register( s0 );

cbuffer LightBuffer
{
	float4 ambient;
	float4 diffuse;
	float3 direction;
	float specular_power;
	float4 specular;
};


struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 view_dir : TEXCOORD1;
};

float4 LightPixelShader( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture.Sample( sample_state, input.tex );
	float4 specular_color = float4( 0.0f, 0.0f, 0.0f, 0.0f );
	float3 light_direction = -direction;
	float intensity = saturate( dot( input.normal, light_direction ) );

	float4 color = ambient;

	if ( intensity > 0.0f )
	{
		color += ( diffuse * intensity );
		
		color = saturate( color );

		float3 reflection = normalize( 2.0f * intensity * input.normal - light_direction );
		specular_color = pow( saturate( dot( reflection, input.view_dir ) ), specular_power );
	}

	color *= src_color;

	color = saturate( color + specular_color );

	return color;
}