#define MAX_LIGHT_NUM 5
float4 GlobalAmbient = {1.0f,1.0f,1.0f,1.0f};

Texture2D Texture : register(t0);
Texture2D NormalTexture : register(t1);
TextureCube shadowTexture : register(t2);
//Texture2D shadowTexture : register(t2);

SamplerState SamplerStateWrap : register(s0);
SamplerState SamplerStateClamp : register(s1);

struct Light
{
	float4 pos;
	float4 dir;
	float4 color;

	float3 att;
	float angle;

	int lightType;
	int enabled;
	int pad;
	int pad2;
};

cbuffer Light : register(b0)
{
    Light light[MAX_LIGHT_NUM];
    int countOfLights;
	int pad;
	int pad1;
	int pad2;
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
};

cbuffer MaterialBuffer : register(b2)
{
	Materials material;
};

struct LightStructure
{
	float4 diffuse;
	float4 specular;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float4 worldPos : POSITION0;
	float4 eyePos : POSITION1;
    float2 texCoord : TEXCOORD;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
	 float4 lightViewPosition : TEXCOORD1;
};

float4 PS(VS_OUTPUT input) : SV_TARGET
{	 

	float3 lightDirection = light[0].pos.xyz - input.worldPos.xyz;
    float shadowdepth = shadowTexture.Sample(SamplerStateClamp, -lightDirection ).r;
	float d = 1 - shadowdepth;
	float le = 1 / d;
    float lightDirectionLength = length(lightDirection);

    //float comparisonResult2 = (le > lightDirectionLength ) ? 1.0f : 0.0f;
	float comparisonResult2 = (le > lightDirectionLength) ? 1.0f : 0.0f;
	comparisonResult2 = 0.01 / (1.01 - shadowdepth);
	//float comparisomResult = !comparisonResult1 || !comparisonResult2;

	return float4(comparisonResult2, comparisonResult2, comparisonResult2, 1.0f);
}