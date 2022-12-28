Texture2D src_texture : register( t0 );

SamplerState sample_state : register( s0 );

cbuffer BlurBuffer : register( b0 )
{
	float2 resolution;
	float weight_count;
	float padding1;
	float4 weights1;
	float4 weights2;
	float4 weights3;
	float4 weights4;
}

float3 blur_x( float2 center )
{
	float weights[ 16 ] = {
		weights1.x, weights1.y, weights1.z, weights1.w,
		weights2.x, weights2.y, weights2.z, weights2.w,
		weights3.x, weights3.y, weights3.z, weights3.w,
		weights4.x, weights4.y, weights4.z, weights4.w,
	};

	float2 resolution_rcp = float2( 1.0f, 1.0f ) / resolution;
	float3 color = float3( 0.0f, 0.0f, 0.0f );
	float2 uv = center;
	int range = weight_count / 2;

	for ( int i = -range; i <= range; ++i )
	{
		uv.x = center.x + i * resolution_rcp.x;
		color += src_texture.Sample( sample_state, uv ).xyz * weights[ i + range ];
	}

	return color;
}

float3 blur_y( float2 center )
{
	float weights[ 16 ] = {
		weights1.x, weights1.y, weights1.z, weights1.w,
		weights2.x, weights2.y, weights2.z, weights2.w,
		weights3.x, weights3.y, weights3.z, weights3.w,
		weights4.x, weights4.y, weights4.z, weights4.w,
	};

	float2 resolution_rcp = float2( 1.0f, 1.0f ) / resolution;
	float3 color = float3( 0.0f, 0.0f, 0.0f );
	float2 uv = center;
	int range = weight_count / 2;

	for ( int i = -range; i <= range; ++i )
	{
		uv.y = center.y + i * resolution_rcp.y;
		color += src_texture.Sample( sample_state, uv ).xyz * weights[ i + range ];
	}

	return color;
}

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 horizontal_blur( PS_INPUT input ) : SV_TARGET
{
	float3 blur_color = blur_x( input.tex );
	float4 color = float4( blur_color, 1.0f );
	return color;
}

float4 vertical_blur( PS_INPUT input ) : SV_TARGET
{
	float3 blur_color = blur_y( input.tex );
	float4 color = float4( blur_color, 1.0f );
	return color;
}