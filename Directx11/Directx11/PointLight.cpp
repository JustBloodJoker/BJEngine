#include "pch.h"
#include "PointLight.h"
#include "Object.h"

namespace BJEngine {


	PointLight::PointLight()
	{
		isLightOn = false;
	}

	PointLight::PointLight(PointLightDesc* descpl)
	{
		this->light = descpl;
		isLightOn = true;
	}

	PointLight::~PointLight()
	{
		DELETE(light);
	}

	bool PointLight::InitLight(ID3D11Device* pd3dDevice)
	{
		if (isLightOn) {
			lightBuffer = Object::InitConstantBuffer<ConstantBufferLight>(pd3dDevice);
			if (lightBuffer == nullptr) {
				Log::Get()->Err("Point light create error");
				return FAILED(E_FAIL);
			}
		}
		return false;
	}

	void PointLight::DrawLight(ID3D11DeviceContext* pImmediateContext)
	{
		if (isLightOn) {

			ConstantBufferLight LIGHT;
			LIGHT.light.ambient = light->ambient;
			LIGHT.light.att = light->att;
			LIGHT.light.diffuse = light->diffuse;
			LIGHT.light.dir = light->dir;
			LIGHT.light.pos = light->pos;
			LIGHT.light.range = light->range;
			
			pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
			pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer);

		}
	}

	void PointLight::SetPos(float x, float y, float z)
	{
		light->pos = dx::XMFLOAT3(x, y, z);
	}

	


}