cbuffer MatrixBuffer
{
	matrix w;
	matrix v;
	matrix p;
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
};

VS_OUTPUT translate_vs( VS_INPUT input )
{
	VS_OUTPUT output;

	output.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	return output;
}
