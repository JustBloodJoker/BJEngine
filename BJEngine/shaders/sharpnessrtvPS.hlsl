struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

Texture2D Texture : register(t0);
SamplerState SamplerStateWrap : register(s0);

static const float offset = 1.0 / 300.0;  

float4 PS(VS_OUTPUT input) : SV_TARGET
{

float2 offsets[9] = {
        float2(-offset,  offset),
        float2( 0.0f,    offset),
        float2( offset,  offset),
        float2(-offset,  0.0f),  
        float2( 0.0f,    0.0f),  
        float2( offset,  0.0f),  
        float2(-offset, -offset),
        float2( 0.0f,   -offset),
        float2( offset, -offset)      
        };
 
    float kernel[9] = {
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    };
    
    float3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = float3(Texture.Sample( SamplerStateWrap, input.texCoord + offsets[i]).xyz);
    }
    float3 col = float3(0.0,0.0,0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    float4 finalColor = float4(col, 1.0);

	return finalColor;
}