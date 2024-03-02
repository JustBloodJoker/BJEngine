#define MAX_LIGHT_NUM 30
#define MAX_SHADOW_NUM 5
#define PI 3.14159265358979323846
float4 GlobalAmbient = {0.1f,0.1f,0.1f,1.0f};

SamplerState SamplerStateWrap : register(s0);
Texture2D PositionScene       : register(t0);
Texture2D DiffuseScene        : register(t1);
Texture2D NormalsScene        : register(t2);
Texture2D RoughnessScene      : register(t3);

SamplerComparisonState SamplerStateClamp             : register(s1);

TextureCube OmnidirectionalShadowMap[MAX_SHADOW_NUM] : register(t5);
Texture2D   SimpleShadowMap         [MAX_SHADOW_NUM] : register(t10);

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

cbuffer Light : register(b0)
{
    Light light[MAX_LIGHT_NUM];
    int countOfLights;
	int pbrEnable;
	int pad1;
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
		if(light[i].enabled)
		{
			LightStructure tcolLight = { {0,0,0,0}, {0,0,0,0} } ;

			if(light[i].lightType == 0)
			{
    			tcolLight = DoSpotLight(light[i], inputNormals, inputWorldPos, inputEyePos);
			}	
			else if (light[i].lightType == 1)
			{
				tcolLight = DoPointLight(light[i], inputNormals, inputWorldPos, inputEyePos);
    		}
			else
			{

			}
			
		
			if(light[i].shadowEnabled)
			{
				if(light[i].lightType == 1)
				{
					float LightDepth = CalculateShadow(inputWorldPos - light[i].pos, 1.0f, 10000.0f);

					switch(sI)
					{
						case 0: depth = OmnidirectionalShadowMap[0].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - light[i].pos.xyz, LightDepth); break;
						case 1: depth = OmnidirectionalShadowMap[1].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - light[i].pos.xyz, LightDepth); break;
						case 2: depth = OmnidirectionalShadowMap[2].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - light[i].pos.xyz, LightDepth); break;
						case 3: depth = OmnidirectionalShadowMap[3].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - light[i].pos.xyz, LightDepth); break;
						case 4: depth = OmnidirectionalShadowMap[4].SampleCmpLevelZero(SamplerStateClamp, inputWorldPos.xyz - light[i].pos.xyz, LightDepth); break;
						default: break;
					}
				}
				else if(light[i].lightType == 0)
				{
					depth = 1.0f;
				}
				sI++;
				tcolLight.diffuse *= depth;
				tcolLight.specular *= depth;
			}
		
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
float DistributionGGX(float3 N, float3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float num = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;
	
    return num / denom;
}
 
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;
 
    float num = NdotV;
    float denom = NdotV * (1.0 - k) + k;
	
    return num / denom;
}
float GeometrySmith(float3 N, float3 V, float3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);
	
    return ggx1 * ggx2;
}
float3 fresnelSchlick(float cosTheta, float3 F0)
{
    return F0 + (1.0 - F0) * pow(1.0 - cosTheta, 5.0);
}  

/////////////////////////////////////////
PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT psOut;

	float4 WorldPosition = PositionScene.Sample( SamplerStateWrap, input.texCoord );
	float4 Normals = NormalsScene.Sample( SamplerStateWrap, input.texCoord );
	float4 ObjectTexture = DiffuseScene.Sample( SamplerStateWrap, input.texCoord );
	float roughnessMap = RoughnessScene.Sample( SamplerStateWrap, input.texCoord ).r;
	float metallicMap = 1.0f - roughnessMap;

	if(pbrEnable)
	{	
		
		float depth = 1.0f;
		int sI = 0;
		float3 N = normalize(Normals.xyz);
    	float3 V = normalize(cameraPos.xyz - WorldPosition.xyz);
		

    	float3 F0 = float3(0.04f,0.04f,0.04f); 
   	 	F0 = lerp(F0, ObjectTexture, metallicMap);
	           
    	float3 Lo = float3(0.0,0.0,0.0);
    	for(int i = 0; i < countOfLights; ++i) 
    	{
			if(light[i].enabled)
			{
        		float3 L = normalize(light[i].pos - WorldPosition).xyz;
        		float distance = length(light[i].pos - WorldPosition);

        		float3 H = normalize(V + L);
        		float attenuation = 1.0f / ( light[i].att.x + light[i].att.y * distance + light[i].att.z * distance * distance );
        		float3 radiance = light[i].color * attenuation;        
        
        		float NDF = DistributionGGX(N, H, roughnessMap);        
        		float G = GeometrySmith(N, V, L, roughnessMap);      
        		float3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);       
        
        		float3 kS = F;
        		float3 kD = float3(1.0,1.0,1.0) - kS;
        		kD *= 1.0 - metallicMap;	  
        
        		float3 numerator = NDF * G * F;
        		float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0);
        		float3 specular = numerator / max(denominator, 0.001);  
            
        		float NdotL = max(dot(N, L), 0.0);                
        		float3 lightRes = (kD * ObjectTexture / PI + specular) * radiance * NdotL; 

				if(light[i].lightType == 0)
				{
					float minCos = cos( light[i].angle );
 			   		float maxCos = ( minCos + 1.0f ) / 2.0f;
    				float cosAngle = dot( light[i].dir, -L );
					float spotIntensity = smoothstep( minCos, maxCos, cosAngle );
					lightRes *= spotIntensity;
				}

				if(light[i].shadowEnabled)
				{
					if(light[i].lightType == 1)
					{
						float LightDepth = CalculateShadow(WorldPosition - light[i].pos, 1.0f, 10000.0f);

						switch(sI)
						{
							case 0: depth = OmnidirectionalShadowMap[0].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - light[i].pos.xyz, LightDepth); break;
							case 1: depth = OmnidirectionalShadowMap[1].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - light[i].pos.xyz, LightDepth); break;
							case 2: depth = OmnidirectionalShadowMap[2].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - light[i].pos.xyz, LightDepth); break;
							case 3: depth = OmnidirectionalShadowMap[3].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - light[i].pos.xyz, LightDepth); break;
							case 4: depth = OmnidirectionalShadowMap[4].SampleCmpLevelZero(SamplerStateClamp, WorldPosition.xyz - light[i].pos.xyz, LightDepth); break;
							default: break;
						}
					}
					else
					{
						depth = 1.0f;
					}
					sI++;
					lightRes *= depth;
				}

				Lo += lightRes;
    		}   
		}
    	float3 ambient = GlobalAmbient.xyz * ObjectTexture.xyz ;
    	float3 color = ambient + Lo;
	  
		psOut.FinalColor.xyz = color;
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