#include "pch.h"
#include "DirectionalLight.h"

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
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ConstantBufferDirectionalLight);
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

	void DirectionalLight::DrawLight(ID3D11DeviceContext* pImmediateContext)
	{
		ConstantBufferDirectionalLight LIGHT;

		if (isLightOn)
		{
			LIGHT.light = *light;
		}

		pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
		pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer);
	}


}