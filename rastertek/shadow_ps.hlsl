Texture2D src_texture : register( t0 );
Texture2D depth_texture : register( t1 );

SamplerState clamp_state : register ( s0 );
SamplerState wrap_state : register( s1 );

cbuffer LightBuffer : register( b0 )
{
	float4 ambient;
	float4 diffuse;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 light_view_pos : TEXCOORD1;
	float3 light_pos : TEXCOORD2;
};


float4 shadow_ps( PS_INPUT input ) : SV_TARGET
{
	float bias = 0.001f;
	float4 color = ambient;

	float2 project_tex;
	project_tex.x = input.light_view_pos.x / input.light_view_pos.w / 2.0f + 0.5f;
	project_tex.y = -input.light_view_pos.y / input.light_view_pos.w / 2.0f + 0.5f;

	float light_depth_value = 0.0f;
	float2 compare_tex = saturate( project_tex );
	if ( compare_tex.x == project_tex.x && compare_tex.y == project_tex.y )
	{
		float depth_value = depth_texture.Sample( clamp_state, project_tex ).r;
		light_depth_value = input.light_view_pos.z / input.light_view_pos.w;
		light_depth_value -= bias;

		if ( light_depth_value < depth_value )
		{
			float light_intensity = saturate( dot( input.normal, input.light_pos ) );

			if ( light_intensity > 0.0f )
			{
				color += ( diffuse * light_intensity );
				color = saturate( color );
			}
		}
	}

	float4 src_color = src_texture.Sample( wrap_state, input.tex );

	color *= src_color;

	return color;
}