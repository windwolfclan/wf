cbuffer MatrixBuffer :register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
};


cbuffer NoiseBuffer : register( b1 )
{
	float frame_time;
	float3 scroll_speed;
	float3 scales;
	float padding;
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
	float2 coord1 : TEXCOORD1;
	float2 coord2 : TEXCOORD2;
	float2 coord3 : TEXCOORD3;
};


VS_OUTPUT fire_vs( VS_INPUT input )
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	output.coord1 = input.tex * scales.x;
	output.coord1.y += frame_time * scroll_speed.x;

	output.coord2 = input.tex * scales.y;
	output.coord2.y += frame_time * scroll_speed.y;

	output.coord3 = input.tex * scales.z;
	output.coord3.y += frame_time * scroll_speed.z;

	return output;
}