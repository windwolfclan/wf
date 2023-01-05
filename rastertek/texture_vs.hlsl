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

VS_OUTPUT TextureVertexShader( VS_INPUT input )
{
	VS_OUTPUT output;

	output.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	return output;
}


struct IVS_INPUT
{
	float4 pos : POSITION;
	float4 tex : TEXCOORD0;
	float3 instance_pos : TEXCOORD1;
};

VS_OUTPUT instance_texture_vs( IVS_INPUT input )
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	input.pos.x += input.instance_pos.x;
	input.pos.y += input.instance_pos.y;
	input.pos.z += input.instance_pos.z;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	return output;
}