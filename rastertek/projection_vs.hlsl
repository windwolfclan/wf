
cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;

	matrix v2;
	matrix p2;
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