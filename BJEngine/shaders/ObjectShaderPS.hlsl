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
	float4 finalColor : SV_TARGET0;
	float4 lightColor : SV_TARGET1;
};

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
	int pad3;
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

cbuffer WorldMatrixBuffer : register(b1)
{
    matrix WVP;
    matrix world;
    matrix cam;
    matrix projection;
    float4 lightpos;
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

float4 CalculateDiffuse(Light lightT, float3 normals, float3 L)
{
	return lightT.color * max( 0, dot( normals, L ) );
}

float4 CalculateSpecular(Light lightT, float3 normals, float3 eye, float3 L)
{

	//BLINN PHONG

	float3 halfwayDir = normalize(L + eye);  
 	float spec = pow(max(dot(normals, halfwayDir), 0.0), 16.0f);


	// PHONG
	//float3 reflectDir = reflect(-L, normals);
    //float   spec = pow(max(dot(eye, reflectDir), 0.0), 8.0);

	return float4(spec, spec, spec, spec);
}

float CalculateShadow(float4 vec, float n, float f)
{
	float3 AbsVec = abs(vec.xyz);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;

}

LightStructure DoDirectionalLight(Light DirLight, float3 inputNormals, float4 inputEyePos)
{
	LightStructure result;

    float3 L = DirLight.dir.xyz;

    result.diffuse = CalculateDiffuse(DirLight, inputNormals, L) ;
    result.specular = CalculateSpecular(DirLight, inputNormals, inputEyePos, L) ;

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
    result.specular = CalculateSpecular(PointLight, inputNormals, inputEyePos.xyz, L)  * attenuation;

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

    result.diffuse = CalculateDiffuse(SpotLight, inputNormals, L) * attenuation * spotIntensity ;  
    result.specular = CalculateSpecular(SpotLight, inputNormals, inputEyePos.xyz, L) * attenuation * spotIntensity;

    return result;
}

LightStructure CalculateLights(float3 inputNormals, float4 inputWorldPos, float4 inputEyePos, float4 lightviewpos[MAX_LIGHT_NUM])
{
	float depth[MAX_LIGHT_NUM];
	for(int i = 0; i < MAX_LIGHT_NUM; i++)
	{
		if(light[i].lightType == 1)
		{
			float LightDepth = CalculateShadow(inputWorldPos - light[i].pos, 1.0f, 10000.0f);
			depth[i] = OmnidirectionalShadowMap[i].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - light[i].pos.xyz, LightDepth);
		}
		else
		{
			depth[i] = 1.0f;
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
		tcolLight.diffuse *= depth[i];
		tcolLight.specular *= depth[i];

		lightFinalColor.diffuse = saturate(lightFinalColor.diffuse + tcolLight.diffuse); 
		lightFinalColor.specular = saturate(lightFinalColor.specular + tcolLight.specular);
	}

	
	return lightFinalColor;
}

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT psOut;

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
	if(material.isEmissionTexture)
	{
		emTex = EmissionTexture.Sample( SamplerStateWrap, input.texCoord );
	}
	float4 specTex = {0.0f, 0.0f, 0.0f, 1.0f};
	if(material.isSpecularTexture)
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
    float4 ObjectSpecular = LightColor.specular;

	finalColor = saturate(ObjectEmissive +  ObjectAmbient + ObjectColor + ObjectSpecular) * ObjectTexture;
	finalColor.w = ObjectTexture.a; 
	if(material.ishaveTransparency)
	{
		finalColor.w *= material.diffuse.w;
	}
	
	psOut.finalColor = finalColor;
	psOut.lightColor = LightColor.diffuse * LightColor.specular;
	return psOut;
}