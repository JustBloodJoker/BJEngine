#include "pch.h"
#include "ShadowMap.h"

namespace BJEngine {

	ShadowMap::ShadowMap()
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	void ShadowMap::SetLight(Light* light)
	{
		this->light = light;
	}

	bool ShadowMap::InitShadowMap(ID3D11Device* pd3dDevice)
	{

		
		return true;
	}


}