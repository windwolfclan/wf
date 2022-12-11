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
	float3 normal : NORMAL;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};


VS_OUTPUT LightVertexShader( VS_INPUT input )
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );
	
	output.tex = input.tex;

	output.normal = mul( input.normal, ( float3x3 )w );
	output.normal = normalize( output.normal );

	return output;
}