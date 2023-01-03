
cbuffer MatrixBuffer : register(b0)
{
	matrix w;
	matrix v;
	matrix p;
};

struct VS_INPUT
{
	float4 pos : POSITION;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 depth_pos : TEXCOORD0;
};

PS_INPUT depth_vs(VS_INPUT input)
{
	PS_INPUT output = (PS_INPUT)0;

	input.pos.w = 1.0f;

	output.pos = mul(input.pos, w);
	output.pos = mul(output.pos, v);
	output.pos = mul(output.pos, p);

	output.depth_pos = output.pos;

	return output;
}