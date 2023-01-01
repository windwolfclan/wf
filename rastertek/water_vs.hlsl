
cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
};

cbuffer ReflectionBuffer : register( b1 )
{
	matrix r;
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PS_INPUT
{
	float4 pos : sv_POSITION;
	float2 tex : TEXCOORD0;
	float4 reflect_pos : TEXCOORD1;
	float4 refract_pos : TEXCOORD2;
};


PS_INPUT water_vs( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	// reflect_project_world
	matrix rpw = mul( r, p );
	rpw = mul( w, rpw );
	output.reflect_pos = mul( input.pos, rpw );

	// view_project_world
	matrix vpw = mul( v, p );
	vpw = mul( w, vpw );
	output.refract_pos = mul( input.pos, vpw );

	return output;
}