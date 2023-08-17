#pragma once
#include "Light.h"

namespace BJEngine {


    struct PointLightDesc
    {
        PointLightDesc()
        {
            ZeroMemory(this, sizeof(PointLightDesc));
        }

        dx::XMFLOAT3 dir;
        float pad1;
        dx::XMFLOAT3 pos;
        float range;
        dx::XMFLOAT3 att;
        float pad2;
        dx::XMFLOAT4 ambient;
        dx::XMFLOAT4 diffuse;
    };

    class PointLight :
        public Light
    {
    public:

        PointLight();
        PointLight(PointLightDesc* descpl);
        ~PointLight();

        bool InitLight(ID3D11Device* pd3dDevice) override;
        void DrawLight(ID3D11DeviceContext* pImmediateContext) override;
        
    private:
        PointLightDesc* light;

        
    };


}
