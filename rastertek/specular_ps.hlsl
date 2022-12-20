Texture2D src_texture[ 3 ];
SamplerState sample_state : register( s0 );


cbuffer LightBuffer : register( b0 )
{
	float4 diffuse;
	float4 specular;
	float specular_power;
	float3 light_direction;
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float3 view_direction : TEXCOORD1;
};

float4 specular_ps( PS_INPUT input ) : SV_TARGET
{
	float4 texture_color = src_texture[ 0 ].Sample( sample_state, input.tex );
	
	float4 bumpmap = src_texture[ 1 ].Sample( sample_state, input.tex );
	bumpmap = ( bumpmap * 2.0f ) - 1.0f;

	float3 bump_normal = input.normal + bumpmap.x * input.tangent + bumpmap.y * input.binormal;
	bump_normal = normalize( bump_normal );

	float3 direction = -light_direction;

	float light_intensity = saturate( dot( bump_normal, direction ) );

	float4 color = saturate( diffuse * light_intensity );

	color *= texture_color;

	if ( light_intensity > 0.0f )
	{
		float4 specular_intensity = src_texture[ 2 ].Sample( sample_state, input.tex );

		float3 reflection = normalize( 2.0f * light_intensity * bump_normal - direction );

		float4 specular_color = pow( saturate( dot( reflection, input.view_direction ) ), specular_power );

		specular_color *= specular_intensity;

		color = saturate( color + specular_color );
	}

	return color;
}