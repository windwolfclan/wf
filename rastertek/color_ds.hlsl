
cbuffer MatrixBuffer : register( b0 )
{
	matrix w;
	matrix v;
	matrix p;
};

struct HS_OUTPUT
{
	float3 pos : POSITION;
	float4 color : COLOR;
};


struct ConstantOutputType
{
	float edges[ 3 ] : SV_TessFactor;
	float inside : SV_InsideTessFactor;
};


struct PS_OUTPUT
{
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

[domain("tri")]

PS_OUTPUT color_ds( ConstantOutputType input, float3 uvw_coord : SV_DomainLocation, const OutputPatch<HS_OUTPUT, 3> patch )
{
	float3 pos;
	PS_OUTPUT output;

	pos = uvw_coord.x * patch[ 0 ].pos + uvw_coord.y * patch[ 1 ].pos + uvw_coord.z * patch[ 2 ].pos;

	output.pos = mul( float4( pos, 1.0f ), w );
	output.pos = mul( output.pos, v );
	output.pos = mul( output.pos, p );

	output.color = patch[ 0 ].color;

	return output;
}