struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 depth_pos : TEXCOORD0;
};

float4 depth_ps(PS_INPUT input) : SV_TARGET
{
	float depth_value = input.depth_pos.z / input.depth_pos.w;

	float4 color;
	if (depth_value < 0.9f)
	{
		color = float4(1.0f, 0.0f, 0.0f, 1.0f);
	}
	else if (depth_value < 0.925f)
	{
		color = float4(0.0f, 1.0f, 0.0f, 1.0f);
	}
	else
	{
		color = float4(0.0f, 0.0f, 1.0f, 1.0f);
	}
	
	return color;
}