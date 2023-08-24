#include "pch.h"
#include "Light.h"
#include "Object.h"

namespace BJEngine {


    Light::Light()
    {
        isLightOn = false;
    }

    Light::Light(LightDesc* tld)
    {
        lightdesc = tld;
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
            lightBuffer = Object::InitConstantBuffer<ConstantBufferLight>(pd3dDevice);
            if (lightBuffer == nullptr) {
                Log::Get()->Err("Dir light create error");
                return FAILED(E_FAIL);
            }
        
        return false;
    }

    void Light::DrawLight(ID3D11DeviceContext* pImmediateContext)
    {

        ConstantBufferLight LIGHT;

        LIGHT.light = *lightdesc;

        pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &LIGHT, 0, 0);
        pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer);
    }

    void Light::SetPos(float x, float y, float z)
    {
        lightdesc->pos = dx::XMFLOAT3(x, y, z);
    }

    dx::XMFLOAT3 Light::GetPos()
    {
        return lightdesc->pos;
    }

    

}