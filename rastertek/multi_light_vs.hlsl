#define LIGHT_COUNT	4

cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
}

cbuffer LightPositionBuffer : register( b1 )
{
	float4 light_pos[ LIGHT_COUNT ];
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 light_pos1 : TEXCOORD1;
	float3 light_pos2 : TEXCOORD2;
	float3 light_pos3 : TEXCOORD3;
	float3 light_pos4 : TEXCOORD4;
};

VS_OUTPUT multi_light_vs( VS_INPUT input )
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	output.normal = input.normal;
	output.normal = normalize( output.normal );

	float4 world_pos = mul( input.pos, w );

	output.light_pos1.xyz = light_pos[ 0 ].xyz - world_pos.xyz;
	output.light_pos2.xyz = light_pos[ 1 ].xyz - world_pos.xyz;
	output.light_pos3.xyz = light_pos[ 2 ].xyz - world_pos.xyz;
	output.light_pos4.xyz = light_pos[ 3 ].xyz - world_pos.xyz;

	output.light_pos1 = normalize( output.light_pos1 );
	output.light_pos2 = normalize( output.light_pos2 );
	output.light_pos3 = normalize( output.light_pos3 );
	output.light_pos4 = normalize( output.light_pos4 );

	return output;
}