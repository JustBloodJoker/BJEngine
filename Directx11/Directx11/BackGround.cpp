#include "pch.h"
#include "BackGround.h"
#include "Object.h"

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
        D3D11_BUFFER_DESC bd;
        ZeroMemory(&bd, sizeof(bd));

        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(Vertex) * numSphereVertices;
        bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;

        D3D11_SUBRESOURCE_DATA pSystem;
        ZeroMemory(&pSystem, sizeof(pSystem));

        pSystem.pSysMem = &vertices[0];

        HRESULT hr = pd3dDevice->CreateBuffer(&bd, &pSystem, &pVertexBuffer);
        if (FAILED(hr))
        {
            Log::Get()->Err("vertex buffer create error");
            return false;
        }

        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(DWORD) * numSphereFaces * 3;
        bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
        bd.CPUAccessFlags = 0;
        bd.MiscFlags = 0;
        pSystem.pSysMem = &indices[0];
        hr = pd3dDevice->CreateBuffer(&bd, &pSystem, &pIndexBuffer);
        if (FAILED(hr))
        {
            Log::Get()->Err("index buffer create error");
            return false;
        }

        shader->Init(pd3dDevice);

        texture->InitCubeMap(pd3dDevice);

        ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
        cmdesc.FillMode = D3D11_FILL_SOLID;
        cmdesc.FrontCounterClockwise = false;
        cmdesc.CullMode = D3D11_CULL_NONE;
        hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullNone);
        if (FAILED(hr))
        {
            Log::Get()->Err("Create rast state error");
            return false;
        }

        D3D11_DEPTH_STENCIL_DESC dssDesc;
        ZeroMemory(&dssDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
        dssDesc.DepthEnable = true;
        dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
        dssDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

        hr = pd3dDevice->CreateDepthStencilState(&dssDesc, &depthStateLessEqual);

        if(FAILED(hr))
        {
            Log::Get()->Err("Create depth stencil buffer error");
            return false;
        }

        ZeroMemory(&bd, sizeof(bd));
        bd.Usage = D3D11_USAGE_DEFAULT;
        bd.ByteWidth = sizeof(ConstantBuffer);
        bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
        bd.CPUAccessFlags = 0;
        hr = pd3dDevice->CreateBuffer(&bd, NULL, &pConstantBuffer);
        if (FAILED(hr))
        {
            Log::Get()->Err("constant buffer create error");
            return false;
        }

        return true;
    }

    void BackGround::Draw()
    {
        world = dx::XMMatrixIdentity();

        world = dx::XMMatrixScaling(5.0f, 5.0f, 5.0f) * dx::XMMatrixTranslation(dx::XMVectorGetX(cam->GetEyeVector()), dx::XMVectorGetY(cam->GetEyeVector()), dx::XMVectorGetZ(cam->GetEyeVector()));

        UINT stride = sizeof(Vertex);
        UINT offset = 0;

        pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
        pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

        dx::XMMATRIX WVP = world * cam->GetViewMatrix() * projection;
        ConstantBuffer cb;
        cb.WVP = XMMatrixTranspose(WVP);
        cb.World = XMMatrixTranspose(world);
        pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
        pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
        pImmediateContext->PSSetShaderResources(0, 1, &texture->GetTexture());
        pImmediateContext->PSSetSamplers(0, 1, &texture->GetTexSamplerState());

        pImmediateContext->VSSetShader(shader->GetVertexShader(), 0, 0);
        pImmediateContext->PSSetShader(shader->GetPixelShader(), 0, 0);
        pImmediateContext->OMSetDepthStencilState(depthStateLessEqual, 0);
        pImmediateContext->RSSetState(renStateCullNone);
        pImmediateContext->DrawIndexed(numSphereFaces * 3, 0, 0);
        pImmediateContext->OMSetDepthStencilState(NULL, 0);

    }

    void BackGround::Close()
    {
        this->Object::Close();
        RELEASE(renStateCullNone);
        RELEASE(depthStateLessEqual);
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