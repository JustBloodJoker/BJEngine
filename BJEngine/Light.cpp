#include "pch.h"
#include "Light.h"

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
        static bool chb;

        for (int i = 0; i < bufferdesclight.lightsCount; i++)
        {
            ImGui::Begin(std::string("Light " + std::to_string(i)).c_str());
            ImGui::Checkbox("enable", (bool*)&bufferdesclight.light[i].enabled);
            ImGui::Text("Color");
            ImGui::SliderFloat("R", &bufferdesclight.light[i].color.x, 0.0f, 1.0f);
            ImGui::SliderFloat("G", &bufferdesclight.light[i].color.y, 0.0f, 1.0f);
            ImGui::SliderFloat("B", &bufferdesclight.light[i].color.z, 0.0f, 1.0f);

            if (bufferdesclight.light[i].lightType == 0)
            {
                ImGui::Text("Direction");
                ImGui::SliderFloat("X", &bufferdesclight.light[i].dir.x, -1.0f, 1.0f);
                ImGui::SliderFloat("Y", &bufferdesclight.light[i].dir.y, -1.0f, 1.0f);
                ImGui::SliderFloat("Z", &bufferdesclight.light[i].dir.z, -1.0f, 1.0f);
                ImGui::SliderFloat(" cone", &bufferdesclight.light[i].angle, 0.00005f, M_PI / 4.0f, "%.5f");
            }

            ImGui::Text("Position");
            ImGui::InputFloat("X", &bufferdesclight.light[i].pos.x);
            ImGui::InputFloat("Y", &bufferdesclight.light[i].pos.y);
            ImGui::InputFloat("Z", &bufferdesclight.light[i].pos.z);

            ImGui::End();
        }

        pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &bufferdesclight, 0, 0);
        pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer);
    }

    void Light::PackLight()
    {
        if (PackMananger::Get()->GetSavingStatus())
        {
            LightType tmp;
            for (size_t index = 0; index < bufferdesclight.lightsCount; index++)
            {
                tmp.pos = bufferdesclight.light[index].pos;
                tmp.dir = bufferdesclight.light[index].dir;
                tmp.angle = bufferdesclight.light[index].angle;
                tmp.att = bufferdesclight.light[index].att;
                tmp.color = bufferdesclight.light[index].color;
                tmp.enabled = bufferdesclight.light[index].enabled;
                tmp.lightType = bufferdesclight.light[index].lightType;
                PackMananger::Get()->AddLight(tmp);
            }

        }

    }

    bool Light::SetLightDesc(LightDesc* tld)
    {
        if (bufferdesclight.lightsCount == MAX_LIGHT_NUM) return false;

        bufferdesclight.light[bufferdesclight.lightsCount] = *tld;
        bufferdesclight.lightsCount++;

        return true;
    }

    void Light::SetPos(float x, float y, float z, int index)
    {
        bufferdesclight.light[index].pos = dx::XMFLOAT4(x, y, z, 1.0f);
    }

    dx::XMFLOAT3 Light::GetPos(int index)
    {
        return dx::XMFLOAT3(bufferdesclight.light[index].pos.x,
            bufferdesclight.light[index].pos.y,
            bufferdesclight.light[index].pos.z);
    }

    dx::XMFLOAT3 Light::GetDir(int index)
    {
        return dx::XMFLOAT3(bufferdesclight.light[index].dir.x,
            bufferdesclight.light[index].dir.y,
            bufferdesclight.light[index].dir.z);
    }

    void Light::InitShadowMap()
    {

    }

    LightDesc Light::GetDesc(int index)
    {
        return  bufferdesclight.light[index];
    }



}