struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

cbuffer OutputPictureProcess : register(b0)
{
	float gamma;
    float exposure;
	int pad1;
	int pad2;
};

Texture2D Texture : register(t0);
SamplerState SamplerStateWrap : register(s0);

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 finalColor = Texture.Sample( SamplerStateWrap, input.texCoord );
    
    // Reinhardt algorithm HDR
    //finalColor = finalColor / (finalColor + float4(1.0f,1.0f,1.0f,1.0f));
    finalColor = float4(1.0f,1.0f,1.0f,1.0f) - exp(-finalColor * exposure);

    finalColor.xyz = pow(finalColor.xyz, float3(1.0f/gamma, 1.0f/gamma,1.0f/gamma));

	return finalColor;
}