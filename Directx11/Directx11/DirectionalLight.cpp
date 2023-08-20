#include "pch.h"
#include "DirectionalLight.h"
#include "Object.h"

namespace BJEngine {


	DirectionalLight::DirectionalLight()
	{
		isLightOn = false;
	}

	DirectionalLight::DirectionalLight(DirectionalLightDesc* light)
	{
		this->light = light;
		isLightOn = true;
	}

	DirectionalLight::~DirectionalLight()
	{
		DELETE(light);
	}

	bool DirectionalLight::InitLight(ID3D11Device* pd3dDevice)
	{
		if (isLightOn) {
			lightBuffer = Object::InitConstantBuffer<ConstantBufferLight>(pd3dDevice);
			if (lightBuffer == nullptr) {
				Log::Get()->Err("Dir light create error");
				return FAILED(E_FAIL);
			}
		}
		return false;
			
	}

	void DirectionalLight::DrawLight(ID3D11DeviceContext* pImmediateContext)
	{
		ConstantBufferLight LIGHT;

		if (isLightOn)
		{
			LIGHT.light.ambient = light->ambient;
			LIGHT.light.diffuse = light->diffuse;
			LIGHT.light.dir = light->dir;
		}

		pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
		pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer);
	}


}