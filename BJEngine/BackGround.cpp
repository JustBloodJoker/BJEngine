#include "BackGround.h"

namespace BJEngine {

    BackGround::BackGround()
    {
        CreateSphere();
    }

    BackGround::~BackGround()
    {
    }

    bool BackGround::Init()
    {
        HRESULT hr = S_OK;

        pVertexBuffer = Helper::InitVertexBuffer(GP::GetDevice(), sizeof(BJEStruct::VertexBackGround) * numSphereVertices, &vertices[0]);
        pIndexBuffer = Helper::InitIndicesBuffer(GP::GetDevice(), sizeof(DWORD) * numSphereFaces * 3, &indices[0]);

        texture->InitCubeMap();

        if (FAILED(hr))
        {
            Log::Get()->Err("Create depth stencil buffer error");
            return false;
        }


        ConstantBuffers=Helper::InitConstantBuffer<BJEStruct::VertexConstantBuffer>(GP::GetDevice());

        Log::Get()->Debug("CubeMap is inited");
        isInited = true;

        

        return true;
    }
    void BackGround::SetTexture(Textures* texture)
    {
        this->texture = texture; hastext = true;
    }
    void BackGround::Draw(const CameraDesc cam)
    {
        GP::BindShader(GP::BACKGROUND_SHADER);
       
        GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        world = dx::XMMatrixIdentity();
        
        world = dx::XMMatrixScaling(5.0f, 5.0f, 5.0f) * 
          
              dx::XMMatrixTranslation(dx::XMVectorGetX(cam.eye), dx::XMVectorGetY(cam.eye), dx::XMVectorGetZ(cam.eye));
        
        UINT stride = sizeof(BJEStruct::VertexBackGround);
        UINT offset = 0;

        GP::GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        GP::GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

        dx::XMMATRIX WVP = world * cam.viewMatrix * cam.projectionMatrix;
        BJEStruct::VertexConstantBuffer cb;
        cb.WVP = XMMatrixTranspose(WVP);
        cb.World = XMMatrixTranspose(world);
        GP::GetDeviceContext()->UpdateSubresource(ConstantBuffers, 0, NULL, &cb, 0, 0);
        GP::GetDeviceContext()->VSSetConstantBuffers(0, 1, &ConstantBuffers);
        GP::GetDeviceContext()->PSSetShaderResources(0, 1, &texture->GetTexture());
        GP::GetDeviceContext()->PSSetSamplers(0, 1, Textures::GetWrapState());

        DepthStencil::SetDepthStencilState(LESS_EQUAL);
       
        Blend::Get()->DrawNoBlend();
        Blend::Get()->DrawCullStateClockFalse();
        GP::GetDeviceContext()->DrawIndexed(numSphereFaces * 3, 0, 0);
        GP::GetDeviceContext()->OMSetDepthStencilState(NULL, 0);
    }

    void BackGround::Close()
    {
        this->Object::Close();
        CLOSE(texture);
        vertices.clear();
        indices.clear();
    }

    void BackGround::CreateSphere()
    {
        vertices.resize(numSphereVertices);
        indices.resize(numSphereFaces * 3);

        float sphereYaw = 0.0f;
        float spherePitch = 0.0f;

        dx::XMVECTOR currVertPos = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);

        vertices[0].pos.x = 0.0f;
        vertices[0].pos.y = 0.0f;
        vertices[0].pos.z = 1.0f;

        for (DWORD i = 0; i < latLines - 2; ++i)
        {
            spherePitch = (i + 1) * (3.14 / (latLines - 1));
            for (DWORD j = 0; j < longLines; ++j)
            {
                sphereYaw = j * (6.28 / (longLines));
                currVertPos = dx::XMVector3TransformNormal(dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f), (dx::XMMatrixRotationX(spherePitch) * dx::XMMatrixRotationZ(sphereYaw)));
                currVertPos = dx::XMVector3Normalize(currVertPos);
                vertices[i * longLines + j + 1].pos.x = dx::XMVectorGetX(currVertPos);
                vertices[i * longLines + j + 1].pos.y = dx::XMVectorGetY(currVertPos);
                vertices[i * longLines + j + 1].pos.z = dx::XMVectorGetZ(currVertPos);
            }
        }

        vertices[numSphereVertices - 1].pos.x = 0.0f;
        vertices[numSphereVertices - 1].pos.y = 0.0f;
        vertices[numSphereVertices - 1].pos.z = -1.0f;

        int k = 0;
        for (DWORD l = 0; l < longLines - 1; ++l)
        {
            indices[k] = 0;
            indices[k + 1] = l + 1;
            indices[k + 2] = l + 2;
            k += 3;
        }

        indices[k] = 0;
        indices[k + 1] = longLines;
        indices[k + 2] = 1;
        k += 3;

        for (DWORD i = 0; i < latLines - 3; ++i)
        {
            for (DWORD j = 0; j < longLines - 1; ++j)
            {
                indices[k] = i * longLines + j + 1;
                indices[k + 1] = i * longLines + j + 2;
                indices[k + 2] = (i + 1) * longLines + j + 1;

                indices[k + 3] = (i + 1) * longLines + j + 1;
                indices[k + 4] = i * longLines + j + 2;
                indices[k + 5] = (i + 1) * longLines + j + 2;

                k += 6;
            }

            indices[k] = (i * longLines) + longLines;
            indices[k + 1] = (i * longLines) + 1;
            indices[k + 2] = ((i + 1) * longLines) + longLines;

            indices[k + 3] = ((i + 1) * longLines) + longLines;
            indices[k + 4] = (i * longLines) + 1;
            indices[k + 5] = ((i + 1) * longLines) + 1;

            k += 6;
        }

        for (DWORD l = 0; l < longLines - 1; ++l)
        {
            indices[k] = numSphereVertices - 1;
            indices[k + 1] = (numSphereVertices - 1) - (l + 1);
            indices[k + 2] = (numSphereVertices - 1) - (l + 2);
            k += 3;
        }

        indices[k] = numSphereVertices - 1;
        indices[k + 1] = (numSphereVertices - 1) - longLines;
        indices[k + 2] = numSphereVertices - 2;




    }


}