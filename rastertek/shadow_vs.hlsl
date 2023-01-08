cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
	matrix lv;
	matrix lp;
}

cbuffer LightBuffer : register( b1 )
{
	float3 light_pos;
	float padding;
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 light_view_pos : TEXCOORD1;
	float3 light_pos : TEXCOORD2;
};

PS_INPUT shadow_vs( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	input.pos.w = 1.0f;

	float4 world_pos = mul( input.pos, w );

	output.pos = mul( world_pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.light_view_pos = mul( world_pos, w );
	output.light_view_pos = mul( output.light_view_pos, lv );
	output.light_view_pos = mul( output.light_view_pos, lp );

	output.tex = input.tex;

	output.normal = mul( input.normal, ( float3x3 )w );
	output.normal = normalize( output.normal );

	output.light_pos = light_pos.xyz - world_pos.xyz;
	output.light_pos = normalize( output.light_pos );

	return output;
}