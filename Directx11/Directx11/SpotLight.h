#pragma once
#include "pch.h"
#include "Light.h"

namespace BJEngine {


    struct SpotLightDesc
    {
        SpotLightDesc()
        {
            ZeroMemory(this, sizeof(Light));
        }
        dx::XMFLOAT3 pos;
        float range;
        dx::XMFLOAT3 dir;
        float cone;
        dx::XMFLOAT3 att;
        float pad2;
        dx::XMFLOAT4 ambient;
        dx::XMFLOAT4 diffuse;
    };

    class SpotLight :
        public Light
    {
    public:

        SpotLight();
        SpotLight(SpotLightDesc* descpl);
        ~SpotLight();

        bool InitLight(ID3D11Device* pd3dDevice) override;

        void DrawLight(ID3D11DeviceContext* pImmediateContext) override;
        
        void SetPos(float x, float y, float z);
    private:

        SpotLightDesc* light;

        struct ConstantBufferSpotLight
        {
            SpotLightDesc light;
        };

    };


}