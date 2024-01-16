struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 finalColor = float4(0.5f, 0.0f, 0.0f, 1.0f);
	
	return finalColor;
}