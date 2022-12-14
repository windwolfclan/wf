cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
};

cbuffer MatrixBuffer : register( b1 )
{
	float4 clip_plane;
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
};

struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float3 normal : NORMAL;
	float clip : SV_ClipDistance0;
};

PS_INPUT refract_vs( VS_INPUT input )
{
	PS_INPUT output = (PS_INPUT)0;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	output.normal = mul( input.normal, ( float3x3 )w );
	output.normal = normalize( output.normal );

	output.clip = dot( mul( input.pos, w ), clip_plane );

	return output;
}