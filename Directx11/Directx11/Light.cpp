#include "pch.h"
#include "Light.h"
#include "Object.h"

namespace BJEngine {


    Light::Light()
    {

    }

    Light::Light(LightDesc* tld)
    {
        bufferdesclight.light[bufferdesclight.lightsCount] = *tld;
        bufferdesclight.lightsCount = 1;
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

    bool Light::InitLight(ID3D11Device* pd3dDevice)
    {
            lightBuffer = Object::InitConstantBuffer<ConstantBufferLight>(pd3dDevice);
            if (lightBuffer == nullptr) {
                Log::Get()->Err("light create error");
                return FAILED(E_FAIL);
            }
        
        return false;
    }

    void Light::DrawLight(ID3D11DeviceContext* pImmediateContext)
    {

       
        pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &bufferdesclight, 0, 0);
        pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer);
    }

    void Light::SetLightDesc(LightDesc* tld)
    {
        if (bufferdesclight.lightsCount == 5) return;

        bufferdesclight.light[bufferdesclight.lightsCount] = *tld;
        bufferdesclight.lightsCount++;
    }

    void Light::SetPos(float x, float y, float z, int index)
    {
        bufferdesclight.light[index].pos = dx::XMFLOAT3(x, y, z);
    }

    dx::XMFLOAT3 Light::GetPos(int index)
    {
        return bufferdesclight.light[index].pos;
    }

    

}