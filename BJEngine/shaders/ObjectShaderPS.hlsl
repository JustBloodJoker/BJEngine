#define MAX_LIGHT_NUM 5
float4 GlobalAmbient = {1.0f,1.0f,1.0f,1.0f};

Texture2D Texture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D RougnessTexture : register(t2);
Texture2D EmissionTexture : register(t3);
Texture2D SpecularTexture : register(t4);

TextureCube OmnidirectionalShadowMap[MAX_LIGHT_NUM] : register(t5);

SamplerState SamplerStateWrap : register(s0);
SamplerComparisonState SamplerStateClamp : register(s1);

struct PS_OUTPUT
{
	float4 OutputPosition : SV_TARGET0;
	float4 OutputDiffuse : SV_TARGET1;
	float4 OutputNormals : SV_TARGET2;
};

struct Materials
{
	float4 emissive;
	float4 ambient;
	float4 diffuse;
	float4 specular;
	float specularPower;
	bool isTexture;
	bool isNormalTexture;
	bool isRoughnessTexture;
	bool isEmissionTexture;
	bool isSpecularTexture;
	bool ishaveTransparency;
};

cbuffer MaterialBuffer : register(b2)
{
	Materials material;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
	float4 eyePos : POSITION1;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
	float3 bitangent : BITANGENT; 
};

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT psOut;

	psOut.OutputDiffuse = material.diffuse;	

	if(material.isTexture)
	{
		psOut.OutputDiffuse = Texture.Sample( SamplerStateWrap, input.texCoord );
	}
	clip(psOut.OutputDiffuse.a < 0.1f ? -1 : 1);
	if(material.ishaveTransparency)
	{
		psOut.OutputDiffuse.w *= material.diffuse.w;
	}

 	input.normal = normalize(input.normal);

	if(material.isNormalTexture )
    {	
		float4 normalMap = NormalTexture.Sample( SamplerStateWrap, input.texCoord );
        normalMap = (2.0f * normalMap) - 1.0f;
		float3x3 texSpace = float3x3(input.tangent, input.bitangent, input.normal);
		input.normal = normalize(mul(normalMap, texSpace));
	}

	psOut.OutputNormals.xyz = input.normal;
	psOut.OutputNormals.w = 1.0f;
	psOut.OutputPosition = input.worldPos;

	return psOut;
}