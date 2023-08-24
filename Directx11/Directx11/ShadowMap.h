#pragma once
#include "pch.h"
#include "Object.h"
#include "Light.h"

namespace BJEngine {

	class ShadowMap
	{
	public:
	
		ShadowMap();
		~ShadowMap();

		void SetLight(Light* light);

		bool InitShadowMap(ID3D11Device* pd3dDevice);

	private:
		Light* light;

		ID3D11Texture2D* shadowMapTexture;
		ID3D11DepthStencilView* shadowMapDSV;
		ID3D11ShaderResourceView* shadowMapSRV;
		ID3D11UnorderedAccessView* shadowMapUAV;
		ID3D11ComputeShader* pComputeShader;
	};


}