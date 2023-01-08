struct PS_INPUT
{
	float4 pos : SV_POSITION;
	float4 depth_pos : TEXCOORD0;
};

float4 depth_ps(PS_INPUT input) : SV_TARGET
{
	float depth_value = input.depth_pos.z / input.depth_pos.w;

	float4 color = float4( depth_value, depth_value, depth_value, 1.0f );
	
	return color;
}