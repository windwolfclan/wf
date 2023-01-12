cbuffer MatrixBuffer
{
	matrix w;
	matrix v;
	matrix p;
};

cbuffer CameraBuffer
{
	float3 camera_pos;
	float padding;
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
	float3 view_dir : TEXCOORD1;
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

	float4 world_pos = mul( input.pos, w );
	output.view_dir = camera_pos - world_pos.xyz;
	output.view_dir = normalize( output.view_dir );

	return output;
}

struct DEFFERED_VS
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct DEFFERED_PS
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

DEFFERED_PS deffered_light_vs( DEFFERED_VS input )
{
	DEFFERED_PS output;

	input.pos.w = 1.0f;

	output.pos = mul( input.pos, w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.tex = input.tex;

	return output;
}