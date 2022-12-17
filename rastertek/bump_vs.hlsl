cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

VS_OUTPUT bump_vs( VS_INPUT input )
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	output.normal = mul( input.normal, ( float3x3 )w );
	output.normal = normalize( output.normal );

	output.tangent = mul( input.tangent, ( float3x3 )w );
	output.tangent = normalize( tangent.normal );

	output.binormal = mul( input.binormal, ( float3x3 )w );
	output.binormal = normalize( binormal.normal );

	return output;
}