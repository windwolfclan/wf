cbuffer MatrixBuffer
{
	matrix w;
	matrix v;
	matrix p;
};

struct VS_INPUT
{
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

VS_OUTPUT ColorVertexShader( VS_INPUT input )
{
	VS_OUTPUT output;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.color = input.color;

	return output;
}


struct HS_INPUT
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

HS_INPUT color_vs2( VS_INPUT input )
{
	HS_INPUT output;

	output.pos = input.pos;
	
	output.color = input.color;

	return output;
}