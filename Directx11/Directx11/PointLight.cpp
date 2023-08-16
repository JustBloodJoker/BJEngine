#include "pch.h"
#include "PointLight.h"

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

			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ConstantBufferPointLight);
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;

			HRESULT hr = pd3dDevice->CreateBuffer(&bd, NULL, &lightBuffer);
			if (FAILED(hr)) {
				Log::Get()->Err("Point light create error");
			}
			return FAILED(hr);

		}
		else
			return true;
	}

	void PointLight::DrawLight(ID3D11DeviceContext* pImmediateContext)
	{
		if (isLightOn) {

			ConstantBufferPointLight LIGHT;
			LIGHT.light = *light;
			pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
			pImmediateContext->PSSetConstantBuffers(1, 1, &lightBuffer);

		}
	}

	void PointLight::DrawLight(ID3D11DeviceContext* pImmediateContext, float x, float y, float z)
	{
		if (isLightOn) {

			ConstantBufferPointLight LIGHT;
			LIGHT.light = *light;
			LIGHT.light.pos.x = x;
			LIGHT.light.pos.y = y;
			LIGHT.light.pos.z = z;
			pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
			pImmediateContext->PSSetConstantBuffers(1, 1, &lightBuffer);

		}
	}


}