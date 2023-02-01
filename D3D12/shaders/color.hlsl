
cbuffer ConstantBufferPerObject : register( b0 )
{
	float4x4 g_wvp;
}

struct VS_IN
{
	float3 pos : POSITION;
	float4 color : COLOR;
};

struct PS_IN
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};


PS_IN vs( VS_IN input )
{
	PS_IN output;

	output.pos = mul( float4( input.pos, 1.0f ), g_wvp );

	output.color = input.color;

	return output;
}

float4 ps( PS_IN input ) : SV_TARGET
{
	return input.color;
}