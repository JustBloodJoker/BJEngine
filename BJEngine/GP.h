#pragma once
#include "pch.h"



namespace GP
{

	enum SHADER_TYPE
	{
		MODEL_SHADER,
		GLOW_MODEL_SHADER,
		BACKGROUND_SHADER,
		
	};

	ID3D11Device*& GetDevice();
	ID3D11DeviceContext*& GetDeviceContext();
	void BindShader(SHADER_TYPE type);
	bool InitShaders();

	bool ClearGlobalParameters();

}


