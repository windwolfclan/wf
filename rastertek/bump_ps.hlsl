Texture2D src_texture[ 2 ] : register( t0 );
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
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

float4 bump_ps( PS_INPUT input ) : SV_TARGET
{
	float4 src_color = src_texture[ 0 ].Sample( sample_state, input.tex );
	float4 bump_color = src_texture[ 1 ].Sample( sample_state, input.tex );

	// expand the range of the normal value from( 0, 1 ) to( -1, 1 )
	bump_color = ( bump_color * 2.0f ) - 1.0f;

	float3 bump_normal = ( bump_color.x * input.tangent ) + ( bump_color.y * input.binormal ) + ( bump_color.z * input.normal );
	bump_normal = normalize( bump_normal );

	float3 light_direction = -direction;

	float light_intensity = saturate( dot( bump_normal, light_direction ) );
	
	float4 color = saturate( diffuse * light_intensity );

	color *= src_color;

	return color;
}