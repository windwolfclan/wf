cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
};

cbuffer ReflectionBuffer : register( b1 )
{
	matrix reflection_matrix;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflection_position : TEXCOORD1;
};


VS_OUTPUT reflection_vs( VS_INPUT input )
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	matrix reflect = mul( reflection_matrix, p );
	reflect = mul( w, reflect );

	output.reflection_position = mul( input.pos, reflect );

	return output;
}