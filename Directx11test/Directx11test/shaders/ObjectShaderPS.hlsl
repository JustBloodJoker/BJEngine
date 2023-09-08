#define MAX_LIGHT_NUM 5
float4 GlobalAmbient = {0.5f,0.5f,0.5f,1.0f};

Texture2D Texture : register(t0);
Texture2D NormalTexture : register(t1);

SamplerState SamplerStateWrap : register(s0);

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
};

float4 CalculateDiffuse(Light lightT, float3 normals, float3 L)
{
	return lightT.color * max( 0, dot( normals, L ) );
}

float4 CalculateSpecular(Light lightT, float3 normals, float3 eye, float3 L)
{
	float RdotV = max( 0, dot( normalize( reflect( -L, normals ) ), eye ) );

	return lightT.color * pow( RdotV, material.specularPower );
}

LightStructure DoDirectionalLight(Light DirLight, float3 inputNormals, float4 inputEyePos)
{
	LightStructure result;

    float3 L = -DirLight.dir.xyz;

    result.diffuse = CalculateDiffuse(DirLight, inputNormals, L);
    result.specular = CalculateSpecular(DirLight, inputNormals, inputEyePos, L);

    return result;
}

LightStructure DoPointLight(Light PointLight, float3 inputNormals, float4 inputWorldPos, float4 inputEyePos)
{
	LightStructure result;

    float3 L = ( PointLight.pos - inputWorldPos ).xyz;
    float distance = length(L);
    L = L / distance;

    float attenuation = 1.0f / ( PointLight.att.x + PointLight.att.y * distance + PointLight.att.z * distance * distance );

    result.diffuse = CalculateDiffuse(PointLight, inputNormals, L) * attenuation;
    result.specular = CalculateSpecular(PointLight, inputNormals, inputEyePos, L) * attenuation;

    return result;
}

LightStructure DoSpotLight(Light SpotLight, float3 inputNormals, float4 inputWorldPos, float4 inputEyePos)
{
	LightStructure result;

    float3 L = ( SpotLight.pos - inputWorldPos ).xyz;
    float distance = length(L);
    L = L / distance;

    float attenuation = 1.0f / ( SpotLight.att.x + SpotLight.att.y * distance + SpotLight.att.z * distance * distance );
    
	float minCos = cos( SpotLight.angle );
    float maxCos = ( minCos + 1.0f ) / 2.0f;
    float cosAngle = dot( SpotLight.dir, -L );
	float spotIntensity = smoothstep( minCos, maxCos, cosAngle );

    result.diffuse = CalculateDiffuse(SpotLight, inputNormals, L) * attenuation * spotIntensity;  
    result.specular = CalculateSpecular(SpotLight, inputNormals, inputEyePos, L) * attenuation * spotIntensity;

    return result;
}

LightStructure CalculateLights(float3 inputNormals, float4 inputWorldPos, float4 inputEyePos)
{
	LightStructure lightFinalColor = { {0,0,0,0}, {0,0,0,0} };

	for(int i = 0; i < countOfLights; i++)
	{
		LightStructure tcolLight = { {0,0,0,0}, {0,0,0,0} } ;

		if(!light[i].enabled)
			continue;

		if(light[i].lightType == 0)
		{
    		tcolLight = DoSpotLight(light[i], inputNormals, inputWorldPos, inputEyePos);
		}	
		else if (light[i].lightType == 2)
    	{
			tcolLight = DoDirectionalLight(light[i], inputNormals, inputEyePos);
		} 
		else if (light[i].lightType == 1)
		{
			tcolLight = DoPointLight(light[i], inputNormals, inputWorldPos, inputEyePos);
    	} 
		lightFinalColor.diffuse += tcolLight.diffuse;
		lightFinalColor.specular += tcolLight.specular;
	}

	lightFinalColor.diffuse = saturate(lightFinalColor.diffuse);
	lightFinalColor.specular = saturate(lightFinalColor.specular);

	return lightFinalColor;
}

float4 PS(VS_OUTPUT input) : SV_TARGET
{
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float4 ObjectTexture = float4(1,1,1,1);	

	if(material.isTexture)
	{
		ObjectTexture = Texture.Sample( SamplerStateWrap, input.texCoord );
	}
    
	if(material.isNormalTexture)
    {
        float4 normalMap = NormalTexture.Sample( SamplerStateWrap, input.texCoord );
        normalMap = (2.0f * normalMap) - 1.0f;
        input.tangent = normalize(input.tangent - dot(input.tangent, input.normal) * input.normal);
        float3 biTangent = cross(input.normal, input.tangent);
        float3x3 texSpace = float3x3(input.tangent, biTangent, input.normal);
        input.normal = normalize(mul(normalMap, texSpace));
    }

	LightStructure LightColor = { {1,1,1,1}, {1,1,1,1} };
	
	if(countOfLights)
	{
		LightColor = CalculateLights(input.normal, input.worldPos, input.eyePos);
	}

	float4 ObjectColor = material.diffuse * LightColor.diffuse;
	float4 ObjectAmbient = material.ambient * GlobalAmbient;
	float4 ObjectEmissive = material.emissive;
    float4 ObjectSpecular = material.specular * LightColor.specular;

	finalColor = (ObjectColor + ObjectAmbient + ObjectEmissive + ObjectSpecular) * ObjectTexture;

	return finalColor;
}