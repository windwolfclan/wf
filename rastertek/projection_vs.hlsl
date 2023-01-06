
cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;

	matrix v2;
	matrix p2;
}

cbuffer LightPositionBuffer : register( b1 )
{
	float3 position;
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
	float4 view_pos : TEXCOORD1;
};

PS_INPUT projection_vs( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.view_pos = mul( input.pos, w );
	output.view_pos = mul( output.view_pos, v2 );
	output.view_pos = mul( output.view_pos, p2 );

	output.tex = input.tex;

	output.normal = mul( input.normal, ( float3x3 )w );
	output.normal = normalize( output.normal );

	return output;
}

struct PS_INPUT2
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float4 view_pos : TEXCOORD1;
	float3 light_pos : TEXCOORD2;
};

PS_INPUT2 projection_vs2( VS_INPUT input )
{
	PS_INPUT2 output = (PS_INPUT2)0;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.view_pos = mul( input.pos, w );
	output.view_pos = mul( output.view_pos, v2 );
	output.view_pos = mul( output.view_pos, p2 );

	output.tex = input.tex;

	output.normal = mul( input.normal, ( float3x3 )w );
	output.normal = normalize( output.normal );

	float4 world_pos = mul( input.pos, w );
	output.light_pos = position.xyz - world_pos.xyz;
	output.light_pos = normalize( output.light_pos );

	return output;
}