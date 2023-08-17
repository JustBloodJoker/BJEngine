#include "pch.h"
#include "SpotLight.h"

BJEngine::SpotLight::SpotLight()
{
	isLightOn = false;
}

BJEngine::SpotLight::SpotLight(SpotLightDesc* descsl)
{
	this->light = descsl;
	isLightOn = true;

}

BJEngine::SpotLight::~SpotLight()
{
	DELETE(light);
}

bool BJEngine::SpotLight::InitLight(ID3D11Device* pd3dDevice)
{
	if (isLightOn) {

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBufferSpotLight);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		HRESULT hr = pd3dDevice->CreateBuffer(&bd, NULL, &lightBuffer);
		if (FAILED(hr)) {
			Log::Get()->Err("Spot light create error");
		}
		return FAILED(hr);

	}
	else
		return true;
	
}

void BJEngine::SpotLight::DrawLight(ID3D11DeviceContext* pImmediateContext)
{

	if (isLightOn) {

		ConstantBufferSpotLight LIGHT;
		LIGHT.light = *light;
		pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
		pImmediateContext->PSSetConstantBuffers(2, 1, &lightBuffer);

	}


}
