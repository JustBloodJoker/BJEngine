#define MAX_LIGHT_NUM 5
float4 GlobalAmbient = {1.0f,1.0f,1.0f,1.0f};

Texture2D Texture : register(t0);
Texture2D NormalTexture : register(t1);
Texture2D RougnessTexture : register(t2);
Texture2D EmissionTexture : register(t3);
Texture2D SpecularTexture : register(t4);

Texture2D shadowLight1[5] : register(t10);
TextureCube shadowCubeLight1[5] : register(t5);

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
	bool isRoughnessTexture;
	bool isEmissionTexture;
	bool isSpecularTexture;
	bool ishaveTransparency;
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
	float3 bitangent : BITANGENT; 

 	float4 lightViewPosition[MAX_LIGHT_NUM] : TEXCOORD1;
};

float CalculateShadowDepth(int index, float4 lightViewPosition)
{
	float2  projectTexCoord;
	projectTexCoord.x =  lightViewPosition.x / lightViewPosition.w / 2.0f + 0.5f;
    projectTexCoord.y = -lightViewPosition.y / lightViewPosition.w / 2.0f + 0.5f;

	float shadowdepth =  shadowLight1[index].Sample(SamplerStateClamp, projectTexCoord ).r;
	
	float lightDepthValue = lightViewPosition.z / lightViewPosition.w;

	float shadow = (lightDepthValue - 0.0001 < shadowdepth) ? 1.0f : 0.0f;
	return shadow;
}

float CalculateCubeShadowDepth(int index, float4 lightpos, float4 inputWorldPos)
{
	float3 lightDirection = lightpos.xyz - inputWorldPos.xyz;
    float shadowdepth = shadowCubeLight1[index].Sample(SamplerStateClamp, -lightDirection ).r;

	float depth = length(lightDirection);
	float d = 1 - shadowdepth;
	float le = 1 / d;

	float shadow = (le > depth) ? 1.0f : 0.0f;

	shadow = (d < 1/depth + 1 / depth - 0.00072) ? 1.0f : 0.0f;

	return shadow;
}

float4 CalculateDiffuse(Light lightT, float3 normals, float3 L)
{
	return lightT.color * max( 0, dot( normals, L ) );
}

float4 CalculateSpecular(Light lightT, float3 normals, float3 eye, float3 L)
{
	//idk is this working?)

	float3 V = normalize(eye);
	float3 R = reflect(L, normals);
	return pow(dot(V,R), material.specularPower );

	//nah not working
}


LightStructure DoDirectionalLight(Light DirLight, float3 inputNormals, float4 inputEyePos, float depth)
{
	LightStructure result;

    float3 L = DirLight.dir.xyz;

    result.diffuse = CalculateDiffuse(DirLight, inputNormals, L) * depth;
  //  result.specular = CalculateSpecular(DirLight, inputNormals, inputEyePos, L) * depth;

    return result;
}

LightStructure DoPointLight(Light PointLight, float3 inputNormals, float4 inputWorldPos, float4 inputEyePos, float depth)
{
	LightStructure result;

    float3 L = ( PointLight.pos - inputWorldPos ).xyz;
    float distance = length(L);

	float shadow = depth;

    L = L / distance;

	float attenuation = 1.0f / ( PointLight.att.x + PointLight.att.y * distance + PointLight.att.z * distance * distance );

    result.diffuse = CalculateDiffuse(PointLight, inputNormals, L) * attenuation * shadow;
    result.specular = CalculateSpecular(PointLight, inputNormals, inputEyePos.xyz, L)  * attenuation * shadow;

    return result;
}

LightStructure DoSpotLight(Light SpotLight, float3 inputNormals, float4 inputWorldPos, float4 inputEyePos, float depth)
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

    result.diffuse = CalculateDiffuse(SpotLight, inputNormals, L) * attenuation * spotIntensity * depth;  
    result.specular = CalculateSpecular(SpotLight, inputNormals, inputEyePos.xyz, L) * attenuation * spotIntensity * depth;

    return result;
}

LightStructure CalculateLights(float3 inputNormals, float4 inputWorldPos, float4 inputEyePos, float4 lightviewpos[MAX_LIGHT_NUM])
{
	float binaryShadowValues[MAX_LIGHT_NUM];
	for(int index = 0; index < MAX_LIGHT_NUM; index++)
	{
		if(light[index].lightType == 1)
		{
			binaryShadowValues[index] = CalculateCubeShadowDepth(index, light[index].pos, inputWorldPos);
		} 
		else
		{
			binaryShadowValues[index] = CalculateShadowDepth(index, lightviewpos[index]);
		}
	}
	
	LightStructure lightFinalColor = { {0,0,0,0}, {0,0,0,0} };

	for(int i = 0; i < countOfLights; i++)
	{
		LightStructure tcolLight = { {0,0,0,0}, {0,0,0,0} } ;

		if(!light[i].enabled)
			continue;

		if(light[i].lightType == 0)
		{
    		tcolLight = DoSpotLight(light[i], inputNormals, inputWorldPos, inputEyePos, binaryShadowValues[i]);
		}	
		else if (light[i].lightType == 2)
    	{
			tcolLight = DoDirectionalLight(light[i], inputNormals, inputEyePos, binaryShadowValues[i]);
		} 
		else if (light[i].lightType == 1)
		{
			tcolLight = DoPointLight(light[i], inputNormals, inputWorldPos, inputEyePos, binaryShadowValues[i]);
    	} 
		lightFinalColor.diffuse = saturate(lightFinalColor.diffuse + tcolLight.diffuse); 
		lightFinalColor.specular = saturate(lightFinalColor.specular + tcolLight.specular);
	}

	
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

	
		clip(ObjectTexture.a < 0.1f ? -1 : 1);
	
 	input.normal = normalize(input.normal);

	if(material.isNormalTexture )
    {
        float4 normalMap = NormalTexture.Sample( SamplerStateWrap, input.texCoord );
        normalMap = (2.0f * normalMap) - 1.0f;
		float3x3 texSpace = float3x3(input.tangent, input.bitangent, input.normal);
		input.normal = normalize(mul(normalMap, texSpace));
	}
	float roughness = 0.0f;
	if(material.isRoughnessTexture)
	{
		roughness = RougnessTexture.Sample( SamplerStateWrap, input.texCoord ).r;
	}
	
	float4 emTex = {1.0f, 1.0f,1.0f,1.0f};
	if(material.isEmissionTexture && pad)
	{
		emTex = EmissionTexture.Sample( SamplerStateWrap, input.texCoord );
	}
	float4 specTex = {0.0f, 0.0f, 0.0f, 1.0f};
	if(material.isSpecularTexture && pad)
	{
		specTex = SpecularTexture.Sample( SamplerStateWrap, input.texCoord );
	}

	LightStructure LightColor = { {1,1,1,1}, {1,1,1,1} };



	if(countOfLights)
	{
		LightColor = CalculateLights(input.normal, input.worldPos, input.eyePos, input.lightViewPosition);
	}

	float4 ObjectColor = material.diffuse * LightColor.diffuse;
	float4 ObjectAmbient = material.ambient * GlobalAmbient;
	float4 ObjectEmissive = emTex * material.emissive;
    float4 ObjectSpecular = specTex  * LightColor.specular;

	finalColor = saturate(ObjectEmissive +  ObjectAmbient + ObjectColor + ObjectSpecular) * ObjectTexture;
	finalColor.w = ObjectTexture.a; 
	if(material.ishaveTransparency)
	{
		finalColor.w *= material.diffuse.w;
	}

	
	return finalColor;
}