struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D Texture : register(t0);
SamplerState SamplerStateWrap : register(s0);

float4 PS(VS_OUTPUT input) : SV_TARGET
{

    float4 finalColor = Texture.Sample( SamplerStateWrap, input.texCoord );
    float average = (finalColor.r + finalColor.g + finalColor.b) / 3.0;
    finalColor = float4(average, average, average, finalColor.a);

	return finalColor;
}