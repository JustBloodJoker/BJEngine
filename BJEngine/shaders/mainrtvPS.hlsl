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
	return finalColor;
}