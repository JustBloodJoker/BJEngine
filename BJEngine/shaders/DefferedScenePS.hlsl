#define MAX_LIGHT_NUM 30
#define MAX_SHADOW_NUM 5

float4 GlobalAmbient = {0.1f,0.1f,0.1f,1.0f};

SamplerState SamplerStateWrap : register(s0);
Texture2D PositionScene : register(t0);
Texture2D DiffuseScene  : register(t1);
Texture2D NormalsScene  : register(t2);


SamplerComparisonState SamplerStateClamp : register(s1);
TextureCube OmnidirectionalShadowMap[MAX_SHADOW_NUM] : register(t5);

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
	int pad3;
	int pad1;
	int pad2;
};

cbuffer Camera : register(b1)
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

	//BLINN PHONG

	float3 halfwayDir = normalize(L + eye);  
 	//float spec = pow(max(dot(normals, halfwayDir), 0.0), 16.0f);
	float cosAngIncidence = dot(normals, L);
	cosAngIncidence = clamp(cosAngIncidence, 0, 1);
	float blinnTerm = dot(normals, halfwayDir);
	blinnTerm = clamp(blinnTerm, 0, 1);
	blinnTerm = cosAngIncidence != 0.0 ? blinnTerm : 0.0;
	blinnTerm = pow(blinnTerm, 16.0f);

	// PHONG
	//float3 reflectDir = reflect(-L, normals);
    //float   spec = pow(max(dot(eye, reflectDir), 0.0), 8.0);

	return blinnTerm;
}

LightStructure DoPointLight(Light PointLight, float3 inputNormals, float4 inputWorldPos, float4 inputEyePos)
{
	LightStructure result;

    float3 L = ( PointLight.pos - inputWorldPos ).xyz;
    float distance = length(L);

    L = L / distance;

	float attenuation = 1.0f / ( PointLight.att.x + PointLight.att.y * distance + PointLight.att.z * distance * distance );

    result.diffuse = CalculateDiffuse(PointLight, inputNormals, L) * attenuation;
    result.specular = PointLight.color * CalculateSpecular(PointLight, inputNormals, inputEyePos.xyz, L) * attenuation;

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
    result.specular = SpotLight.color *  CalculateSpecular(SpotLight, inputNormals, inputEyePos.xyz, L) * attenuation * spotIntensity;

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
				else
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

PS_OUTPUT PS(VS_OUTPUT input)
{
	PS_OUTPUT psOut;

	float4 WorldPosition = PositionScene.Sample( SamplerStateWrap, input.texCoord );
	float4 Normals = NormalsScene.Sample( SamplerStateWrap, input.texCoord );
	float4 ObjectTexture = DiffuseScene.Sample( SamplerStateWrap, input.texCoord );

	float4 CameraPos = mul(WorldPosition, camera);

	float4 finalColor = float4(0.0f, 0.0f, 0.0f, 1.0f);


	LightStructure LightColor = { {1,1,1,1}, {1,1,1,1} };
	if(countOfLights)
	{
		LightColor = CalculateLights(Normals.xyz, WorldPosition, normalize(cameraPos - WorldPosition));
	}

	float4 ObjectColor =  LightColor.diffuse;
	float4 ObjectAmbient = GlobalAmbient;
	float4 ObjectSpecular = LightColor.specular;

	finalColor = saturate( ObjectColor + ObjectAmbient + ObjectSpecular) * ObjectTexture;
	finalColor.w = ObjectTexture.w;
	psOut.FinalColor = finalColor;
	
	return psOut;
}