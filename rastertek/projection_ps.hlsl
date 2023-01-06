
Texture2D src_texture : register( t0 );
Texture2D projection_texture : register( t1 );

SamplerState sample_state : register( s0 );

cbuffer LightBuffer : register( b0 )
{
	float4 ambient;
	float4 diffuse;
	float3 direction;
	float padding;
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 view_pos : TEXCOORD1;
};

float4 projection_ps( PS_INPUT input ) : SV_TARGET
{
	float3 light_direction = -direction;
	float light_intensity = saturate( dot( input.normal, light_direction ) );

	float4 color = ambient;
	if ( light_intensity > 0.0f )
	{
		color += ( diffuse * light_intensity );
	}

	color = saturate( color );

	float4 src_color = src_texture.Sample( sample_state, input.tex );
	color *= src_color;

	float2 projection_tex;
	projection_tex.x = input.view_pos.x / input.view_pos.w / 2.0f + 0.5f;
	projection_tex.y = -input.view_pos.y / input.view_pos.w / 2.0f + 0.5f;

	float2 compare_tex = saturate( projection_tex );

	if ( compare_tex.x == projection_tex.x && compare_tex.y == projection_tex.y )
	{
		color = projection_texture.Sample( sample_state, projection_tex );
	}
	
	return color;
}

struct PS_INPUT2
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 view_pos : TEXCOORD1;
	float3 light_pos : TEXCOORD2;
};

float4 projection_ps2( PS_INPUT2 input ) : SV_TARGET
{
	float4 color;
	float brightness = 1.5f;
	float light_intensity = saturate( dot( input.normal, input.light_pos ) );
	if ( light_intensity > 0.0f )
	{
		color = ( diffuse * light_intensity ) * brightness;
	}
	
	float4 src_color = src_texture.Sample( sample_state, input.tex );

	float2 projection_tex;
	projection_tex.x = input.view_pos.x / input.view_pos.w / 2.0f + 0.5f;
	projection_tex.y = -input.view_pos.y / input.view_pos.w / 2.0f + 0.5f;

	float2 compare_tex = saturate( projection_tex );
	if ( compare_tex.x == projection_tex.x && compare_tex.y == projection_tex.y )
	{
		float4 projection_color = projection_texture.Sample( sample_state, projection_tex );
		color = saturate( ( color * projection_color * src_color ) + ( ambient * src_color ) );
	}
	else
	{
		color = ambient * src_color;
	}

	return color;
}