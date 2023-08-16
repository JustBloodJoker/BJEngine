#include "pch.h"
#include "Light.h"

namespace BJEngine {


    Light::Light()
    {
        isLightOn = false;
    }

    Light::~Light()
    {
        RELEASE(lightBuffer);
    }

    void Light::Close()
    {
    }

    ID3D11Buffer* Light::GetConstantBuffer()
    {
        return lightBuffer;
    }

    bool Light::IsLightOn()
    {
        return isLightOn;
    }

    bool Light::InitLight(ID3D11Device* pd3dDevice)
    {
        return true;
    }

    void Light::DrawLight(ID3D11DeviceContext* pImmediateContext)
    {
    }


}