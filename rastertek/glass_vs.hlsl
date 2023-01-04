cbuffer MatrixBuffer : register( b0 )
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

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float4 refract_pos : TEXCOORD1;
};

PS_INPUT glass_vs( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	matrix vpw = mul( v, p );
	vpw = mul( w, vpw );

	output.refract_pos = mul( input.pos, vpw );

	return output;
}