#pragma once
#include "pch.h"



namespace GP
{

	enum SHADER_TYPE
	{
		MODEL_SHADER,
		GLOW_MODEL_SHADER,
		BACKGROUND_SHADER,
		DEFFEREDSCENE_SHADER_PS,
		SPHERE_SHADER,
		OMNIDIRECTIONAL_SHADOW_SHADER,
		SIMPLE_SHADOW_SHADER,
		IRRADIANCE_SKYBOX_TEXTURE,

	};

	ID3D11Device*& GetDevice();
	ID3D11DeviceContext*& GetDeviceContext();
	void BindShader(SHADER_TYPE type);
	bool InitShaders();

	bool ClearGlobalParameters();

	void ResetShaders();
}


