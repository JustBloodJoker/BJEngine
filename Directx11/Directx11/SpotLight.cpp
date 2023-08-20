#include "pch.h"
#include "SpotLight.h"
#include "Object.h"

namespace BJEngine {


	SpotLight::SpotLight()
	{
		isLightOn = false;
	}

	SpotLight::SpotLight(SpotLightDesc* descsl)
	{
		this->light = descsl;
		isLightOn = true;

	}

	SpotLight::~SpotLight()
	{
		DELETE(light);
	}

	bool SpotLight::InitLight(ID3D11Device* pd3dDevice)
	{
		if (isLightOn) {
			lightBuffer = Object::InitConstantBuffer<ConstantBufferLight>(pd3dDevice);
			if (lightBuffer == nullptr) {
				Log::Get()->Err("Spot light create error");
				return FAILED(E_FAIL);
			}
		}
		return false;

	}

	void SpotLight::DrawLight(ID3D11DeviceContext* pImmediateContext)
	{

		if (isLightOn) {

			ConstantBufferLight LIGHT;
			LIGHT.light.ambient = light->ambient;
			LIGHT.light.att = light->att;
			LIGHT.light.cone = light->cone;
			LIGHT.light.diffuse = light->diffuse;
			LIGHT.light.dir = light->dir;
			LIGHT.light.pos = light->pos;
			LIGHT.light.range = light->range;
			LIGHT.light.pad = 0.0f;

			pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
			pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer);

		}


	}

	void SpotLight::SetPos(float x, float y, float z)
	{
		light->pos = dx::XMFLOAT3(x,y,z);
	}


}