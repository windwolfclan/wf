cbuffer PerFrameBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
}

cbuffer FogBuffer : register( b1 )
{
	float fog_start;
	float fog_end;
	float2 padding;
}

struct VS_INPUT
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct VS_OUTPUT
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
	float fog : FOG;
};

VS_OUTPUT fog_vs( VS_INPUT input )
{
	VS_OUTPUT output;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	float4 camera_position = mul( input.pos, w );
	camera_position = mul( camera_position, v );

	output.fog = saturate( ( fog_end - camera_position.z ) / ( fog_end - fog_start ) );

	return output;
}