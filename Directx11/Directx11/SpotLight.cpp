#include "pch.h"
#include "SpotLight.h"


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
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ConstantBufferLight);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			HRESULT hr = pd3dDevice->CreateBuffer(&bd, NULL, &lightBuffer);

			if (FAILED(hr)) {
				Log::Get()->Err("Dir light create error");
			}

			return FAILED(hr);
		}
		else
			return true;

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