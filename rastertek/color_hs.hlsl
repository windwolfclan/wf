cbuffer TessellationBuffer : register( b0 )
{
	float tessellation_amount;
	float3 padding;
};

struct HS_INPUT
{
	float3 pos : POSITION;
	float4 color : COLOR;
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

ConstantOutputType ColorPatchConstantFunction( InputPatch<HS_INPUT, 3> input_patch, uint patch_id : SV_PrimitiveID )
{
	ConstantOutputType output;

	output.edges[ 0 ] = tessellation_amount;
	output.edges[ 1 ] = tessellation_amount;
	output.edges[ 2 ] = tessellation_amount;

	output.inside = tessellation_amount;

	return output;
}

// HULL SHADER
[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("ColorPatchConstantFunction")]

HS_OUTPUT color_hs( InputPatch<HS_INPUT, 3> patch, uint point_id : SV_OutputControlPointID, uint patch_id : SV_PrimitiveID )
{
	HS_OUTPUT output;

	output.pos = patch[ point_id ].pos;

	output.color = patch[ point_id ].color;

	return output;
}