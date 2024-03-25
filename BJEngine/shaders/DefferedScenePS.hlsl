#define MAX_LIGHT_NUM 30
#define MAX_SHADOW_NUM 5
#define PI 3.14159265358979323846
float4 GlobalAmbient = {0.1f,0.1f,0.1f,1.0f};

SamplerState SamplerStateWrap : register(s0);
Texture2D PositionScene       : register(t0);
Texture2D DiffuseScene        : register(t1);
Texture2D NormalsScene        : register(t2);
Texture2D SpecularScene       : register(t3);
Texture2D RoughnessScene      : register(t4);

TextureCube IrradianceScene   : register(t6);

SamplerComparisonState SamplerStateClamp             : register(s1);

TextureCube OmnidirectionalShadowMap[MAX_SHADOW_NUM] : register(t8);
Texture2D   SimpleShadowMap         [MAX_SHADOW_NUM] : register(t13);


struct Light
{
	float4 pos;
	float4 dir;
	float4 color;

	float3 att;
	float angle;

	int lightType;
	int enabled;
	int shadowEnabled;
	int pad2;
};
StructuredBuffer<Light> LightsData : register(t15);

cbuffer Light : register(b0)
{
    int countOfLights;
	int pbrEnable;
	int ibrEnable;
	int pad2;
};

cbuffer Camera : register(b3)
{
	matrix camera;
	float4 cameraPos;
}

struct LightStructure
{
	float4 diffuse;
	float4 specular;
};

struct PS_OUTPUT
{
	float4 FinalColor : SV_TARGET0;
};

struct VS_OUTPUT
{
    float4 pos : SV_POSITION;
    float2 texCoord : TEXCOORD;
};

float CalculateShadow(float4 vec, float n, float f)
{
	float3 AbsVec = abs(vec.xyz);
    float LocalZcomp = max(AbsVec.x, max(AbsVec.y, AbsVec.z));

    float NormZComp = (f+n) / (f-n) - (2*f*n)/(f-n)/LocalZcomp;
    return (NormZComp + 1.0) * 0.5;

}

float4 CalculateDiffuse(Light lightT, float3 normals, float3 L)
{
	return lightT.color * max( 0, dot( normals, L ) );
}

float CalculateSpecular(Light lightT, float3 normals, float3 eye, float3 L)
{
	float3 halfwayDir = normalize(L + eye);  
 	float spec = pow(max(dot(normals, halfwayDir), 0.0), 16.0f);
	return spec;
}

LightStructure DoPointLight(Light PointLight, float3 inputNormals, float4 inputWorldPos, float4 inputEyePos)
{
	LightStructure result;

    float3 L = ( PointLight.pos - inputWorldPos ).xyz;
    float distance = length(L);
    L = L / distance;

	float attenuation = 1.0f / ( PointLight.att.x + PointLight.att.y * distance + PointLight.att.z * distance * distance );
	
	float3 E = (inputEyePos - inputWorldPos).xyz;
	distance = length(E);
	E = E / distance;

    result.diffuse = CalculateDiffuse(PointLight, inputNormals, L) * attenuation;
    result.specular = PointLight.color * CalculateSpecular(PointLight, inputNormals, E, L) * attenuation;

    return result;
}

LightStructure DoSpotLight(Light SpotLight, float3 inputNormals, float4 inputWorldPos, float4 inputEyePos)
{
	LightStructure result;

    float3 L = ( SpotLight.pos - inputWorldPos ).xyz;
    float distance = length(L);
    L = L / distance;

    float attenuation = 1.0f / ( SpotLight.att.x + SpotLight.att.y * distance + SpotLight.att.z * distance * distance );
    
	float3 E = (inputEyePos - inputWorldPos).xyz;
	distance = length(E);
	E = E / distance;


	float minCos = cos( SpotLight.angle );
    float maxCos = ( minCos + 1.0f ) / 2.0f;
    float cosAngle = dot( SpotLight.dir, -L );
	float spotIntensity = smoothstep( minCos, maxCos, cosAngle );

    result.diffuse = CalculateDiffuse(SpotLight, inputNormals, L) * attenuation * spotIntensity ;  
    result.specular = SpotLight.color *  CalculateSpecular(SpotLight, inputNormals, E, L) * attenuation * spotIntensity;

    return result;
}

LightStructure CalculateLights(float3 inputNormals, float4 inputWorldPos, float4 inputEyePos)
{
	float depth = 1.0f;
	int sI = 0;
	
	
	LightStructure lightFinalColor = { {0,0,0,0}, {0,0,0,0} };

	for(int i = 0; i < countOfLights; i++)
	{
		if(LightsData[i].enabled)
		{
			LightStructure tcolLight = { {0,0,0,0}, {0,0,0,0} } ;

			if(LightsData[i].lightType == 0)
			{
    			tcolLight = DoSpotLight(LightsData[i], inputNormals, inputWorldPos, inputEyePos);
			}	
			else if (LightsData[i].lightType == 1)
			{
				tcolLight = DoPointLight(LightsData[i], inputNormals, inputWorldPos, inputEyePos);
    		}
			else
			{

			}
			
		
			if(LightsData[i].shadowEnabled)
			{
				if(LightsData[i].lightType == 1)
				{
					float LightDepth = CalculateShadow(inputWorldPos - LightsData[i].pos, 1.0f, 10000.0f);

					switch(sI)
					{
						case 0: depth = OmnidirectionalShadowMap[0].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 1: depth = OmnidirectionalShadowMap[1].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 2: depth = OmnidirectionalShadowMap[2].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 3: depth = OmnidirectionalShadowMap[3].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 4: depth = OmnidirectionalShadowMap[4].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - LightsData[i].pos.xyz, LightDepth); break;
						default: break;
					}
				}
				else if(LightsData[i].lightType == 0)
				{
					depth = 1.0f;
				}
				sI++;
				
			}
			tcolLight.diffuse *= depth;
			tcolLight.specular *= depth;


			lightFinalColor.diffuse = saturate(lightFinalColor.diffuse + tcolLight.diffuse); 
			lightFinalColor.specular = saturate(lightFinalColor.specular + tcolLight.specular);
		
		}	
		else
		{

		}
	}

	
	return lightFinalColor;
}



float4 DefaultLight(float3 normals, float4 worldP, float4 camPos)
{
	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);
	LightStructure LightColor = { {1,1,1,1}, {1,1,1,1} };
	if(countOfLights)
	{
		LightColor = CalculateLights(normals, worldP, camPos);
	}
	float4 ObjectColor =  LightColor.diffuse;
	float4 ObjectAmbient = GlobalAmbient;
	float4 ObjectSpecular = LightColor.specular;

	finalColor = saturate( ObjectColor + ObjectAmbient + ObjectSpecular);
	return finalColor;
}

/////////////////////////////////////////

float GGX_PartialGeometry(float cosThetaN, float alpha) {
    float cosTheta_sqr = saturate(cosThetaN*cosThetaN);
    float tan2 = ( 1 - cosTheta_sqr ) / cosTheta_sqr;
    float GP = 2 / ( 1 + sqrt( 1 + alpha * alpha * tan2 ) );
    return GP;
}

float GGX_Distribution(float cosThetaNH, float alpha) {
    float alpha2 = alpha * alpha;
    float NH_sqr = saturate(cosThetaNH * cosThetaNH);
    float den = NH_sqr * alpha2 + (1.0 - NH_sqr);
    return alpha2 / ( PI * den * den );
}

float3 FresnelSchlick(float3 F0, float cosTheta, float roughness) 
{
    return F0 + (max(float3(1.0 - roughness,1.0 - roughness,1.0 - roughness), F0) - F0) * pow(1.0 - cosTheta, 5.0);
}

float3 PBRLights(Light lightT, float3 V, float3 L, float3 N, float3 ALBEDO, float3 SPEC, float ROUGH, float3 irradiance)
{
	float dist = length(L);
	N = normalize(N);
    V = normalize(V);
    L = normalize(L);
	float3 H = normalize(V+L);
    
	float NL = dot(N, L);
	if (NL <= 0.0) return 0.0;

    float NV = dot(N, V);
	if (NV <= 0.0) return 0.0;
    
	float NH = dot(N, H);
	float HV = dot(H, V);


	float roug_alpha = ROUGH * ROUGH;
	float G = GGX_PartialGeometry(NV, roug_alpha) * GGX_PartialGeometry(NL, roug_alpha);
	float D = GGX_Distribution(NH, roug_alpha);
	float3 F = FresnelSchlick(SPEC, HV, roug_alpha); //SPEC

	float3 resultSpec = G*D*F*0.25/NV;
	float3 resultDiff = saturate(1-F);

	float attenuation = 1.0f / ( lightT.att.x + lightT.att.y * dist + lightT.att.z * dist * dist );

	return max(0.0f, resultSpec * attenuation * lightT.color + resultDiff * ALBEDO * lightT.color * NL * irradiance * attenuation/ PI);
}

float3 PBRCalculate(float3 Normals, float4 WorldPosition, float4 cameraPos, float3 albed, float3 spec,float rough)
{
	float3 finalCol = float3(0.0f, 0.0f, 0.0f);
	int sI = 0;
	float3 IrradianceMap = IrradianceScene.Sample(SamplerStateWrap, Normals).xyz;
	for(int i = 0; i < countOfLights; i++)
	{
		if(LightsData[i].enabled)
		{	
			float depth = 1.0f;
			float3 color = PBRLights(LightsData[i], cameraPos.xyz - WorldPosition.xyz ,LightsData[i].pos.xyz - WorldPosition.xyz , Normals, albed, spec, rough, ibrEnable ? IrradianceMap : 1.0f);
			
			if(LightsData[i].shadowEnabled)
			{
				if(LightsData[i].lightType == 1)
				{
					float LightDepth = CalculateShadow(WorldPosition - LightsData[i].pos, 1.0f, 10000.0f);

					switch(sI)
					{
						case 0: depth = OmnidirectionalShadowMap[0].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 1: depth = OmnidirectionalShadowMap[1].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 2: depth = OmnidirectionalShadowMap[2].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 3: depth = OmnidirectionalShadowMap[3].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - LightsData[i].pos.xyz, LightDepth); break;
						case 4: depth = OmnidirectionalShadowMap[4].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - LightsData[i].pos.xyz, LightDepth); break;
						default: break;
					}
				}
				else if(LightsData[i].lightType == 0)
				{
					depth = 1.0f;
				}
				sI++;
			}
			
			color *= depth;

			finalCol = saturate(finalCol + color);
		}
	}
	return finalCol;
}


/////////////////////////////////////////

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT psOut;

	float4 WorldPosition = PositionScene.Sample( SamplerStateWrap, input.texCoord );
	float4 Normals = NormalsScene.Sample( SamplerStateWrap, input.texCoord );
	float4 ObjectTexture = DiffuseScene.Sample( SamplerStateWrap, input.texCoord );
	float4 SpecularTexture = SpecularScene.Sample( SamplerStateWrap, input.texCoord );
	float roughnessMap = RoughnessScene.Sample( SamplerStateWrap, input.texCoord ).r;
	float metallicMap = 1.0f - roughnessMap;

	

	if(pbrEnable)
	{	
		
		
		
		psOut.FinalColor.xyz = PBRCalculate(Normals, WorldPosition, cameraPos, ObjectTexture.xyz,SpecularTexture.xyz, roughnessMap);
		psOut.FinalColor.w = ObjectTexture.w;
	}
	else
	{
		psOut.FinalColor = DefaultLight(Normals.xyz, WorldPosition, cameraPos);
		psOut.FinalColor *= ObjectTexture;
		psOut.FinalColor.w = ObjectTexture.w;
	}
	return psOut;
}