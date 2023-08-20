#include "pch.h"
#include "StaticModelOBJ.h"

namespace BJEngine {

	StaticModelOBJ::StaticModelOBJ()
	{
       
	}

	StaticModelOBJ::StaticModelOBJ(std::wstring objFileName)
	{
        filename = objFileName;
	}

	StaticModelOBJ::~StaticModelOBJ()
	{

	}

	bool StaticModelOBJ::Init()
	{
        LoadObjModel();
        
        D3D11_INPUT_ELEMENT_DESC layout[4] = {
            { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
            { "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
            { "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
        };
        
        shader->SetInputLayout(layout, ARRAYSIZE(layout));
        shader->Init(pd3dDevice);
        
        if(light != nullptr)
        light->InitLight(pd3dDevice);

        InitIsLightConstantBuffer();

        HRESULT hr = S_OK;
        D3D11_BUFFER_DESC bd;
        D3D11_SUBRESOURCE_DATA InitData;

        for (int i = 0; i < packVertices.size(); i++)
        {
            packpVertexBuffer.push_back({});
            packpVertexBuffer[i] = Object::InitVertexBuffer(pd3dDevice, packVertices[i].vertices.size() * sizeof(Vertex), &packVertices[i].vertices[0]);
        }

        pConstantBuffer = Object::InitConstantBuffer<Object::ConstantBuffer>(pd3dDevice);
        
        pAdditionalConstantBuffer = Object::InitConstantBuffer<AdditionalConstantBuffer>(pd3dDevice);

        for (int i = 0; i < packVertices.size(); i++)
        {
            if (mat[packVertices[i].mtlName].textKDName != L"")
            {
                mat[packVertices[i].mtlName].texture = new Textures(mat[packVertices[i].mtlName].textKDName.c_str());

                if (mat[packVertices[i].mtlName].texture->InitTextures(pd3dDevice))
                    return false;
            }

            if (mat[packVertices[i].mtlName].textBumpName != L"")
            {
                mat[packVertices[i].mtlName].textureBump = new Textures(mat[packVertices[i].mtlName].textBumpName.c_str());

                if (mat[packVertices[i].mtlName].textureBump->InitTextures(pd3dDevice))
                    return false;
            }
        }

        ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
        cmdesc.FillMode = D3D11_FILL_SOLID;
        cmdesc.FrontCounterClockwise = true;
        cmdesc.CullMode = D3D11_CULL_NONE;
        hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullNone);
        if (FAILED(hr))
        {
            Log::Get()->Err("Create rast state error");
            return false;
        }

        rotation = dx::XMMatrixRotationY(3.14f);
        scale = dx::XMMatrixScaling(1.0f, 1.0f, 1.0f);
        pos = dx::XMMatrixTranslation(0.0f, 0.0f, 0.0f);
        
        return true;
	}

	void StaticModelOBJ::Draw()
	{

        pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

        world = dx::XMMatrixIdentity();
        
        world = rotation * scale * pos;
        
        UINT stride = sizeof(Vertex);
        UINT offset = 0;
        ConstantBuffer constantBuffer;
        AdditionalConstantBuffer acb;
        DrawIsLightConstantBuffer();

        if (light != nullptr) {
            light->DrawLight(pImmediateContext);
        }

        for (int i = 0; i < packpVertexBuffer.size(); ++i)
        {
            pImmediateContext->IASetInputLayout(shader->GetInputLayout());

            
            pImmediateContext->IASetVertexBuffers(0, 1, &packpVertexBuffer[i], &stride, &offset);

            constantBuffer.WVP = dx::XMMatrixTranspose(world * view * projection);
            constantBuffer.World = dx::XMMatrixTranspose(world);
            
            acb.diffuse = mat[packVertices[i].mtlName].difColor;
            

            if (mat[packVertices[i].mtlName].textKDName != L"") 
            {
                acb.hasText = TRUE;
            }
            else
            {
                acb.hasText = FALSE;
            }

            if (mat[packVertices[i].mtlName].textBumpName != L"")
            {
                acb.hasNormalMap = TRUE;
            }
            else
            {
                acb.hasNormalMap = FALSE;
            }
            pImmediateContext->UpdateSubresource(pAdditionalConstantBuffer, 0, NULL, &acb, 0, 0);
            pImmediateContext->PSSetConstantBuffers(2, 1, &pAdditionalConstantBuffer);
            
            pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &constantBuffer, 0, 0);
            pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

            if (acb.hasText) 
            {
                pImmediateContext->PSSetShaderResources(0, 1, &mat[packVertices[i].mtlName].texture->GetTexture());
                pImmediateContext->PSSetSamplers(0, 1, &mat[packVertices[i].mtlName].texture->GetTexSamplerState());
            }
            if (acb.hasNormalMap) 
            {
                pImmediateContext->PSSetShaderResources(1, 1, &mat[packVertices[i].mtlName].textureBump->GetTexture());
            }

            pImmediateContext->RSSetState(renStateCullNone);
            pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
            pImmediateContext->PSSetShader(shader->GetPixelShader(), NULL, 0);
            pImmediateContext->Draw(packVertices[i].vectorsize, 0);
        }



	}

	void StaticModelOBJ::Close()
	{
        Object::Close();
        RELEASE(pAdditionalConstantBuffer);
        mat.clear();
        packVertices.clear();
        objs.clear();

        for (auto& el : packpVertexBuffer)
            RELEASE(el);
        packpVertexBuffer.clear();
        for (auto& el : packpIndexBuffer)
            RELEASE(el);
        packpIndexBuffer.clear();

        RELEASE(Transparency);


    }

	bool StaticModelOBJ::LoadObjModel()
	{
        file.open(filename);

        char tchar;
        std::wstring lastmaterial;

        if (file)
        {
            while (file) {

                tchar = file.get();
                switch (tchar)
                {
                case '#':

                    while (tchar != '\n')
                    {
                        tchar = file.get();
                    }
                    break;

                case 'm':
                    tchar = file.get();
                    if (tchar == 't')  tchar = file.get();
                        if (tchar == 'l')  tchar = file.get();
                            if (tchar == 'l')  tchar = file.get();
                                if (tchar == 'i')  tchar = file.get();
                                    if (tchar == 'b')
                                    {
                                        tchar = file.get();
                                        while (tchar != '\n')
                                        {
                                            tchar = file.get();
                                            if (tchar == '\n') continue;

                                            mtlfilename += tchar;
                                        }
                                    }
                                    break;

                case 'o':
                    objs.push_back({});
                    tchar = file.get();
                    while (tchar != '\n') {
                        tchar = file.get();
                        if (tchar == '\n') continue;

                        objs[objs.size() - 1].nameObject += tchar;
                    }
                    break;


                case 'v':
                    tchar = file.get();
                    if (tchar == ' ')
                    {
                        float x, y, z;
                        file >> x >> y >> z;

                        objs[objs.size() - 1].vertPos.push_back(dx::XMFLOAT3(x, y, z));
                    }
                    if (tchar == 't')
                    {
                        float u, v;
                        file >> u >> v;

                        objs[objs.size() - 1].vertTexCoord.push_back(dx::XMFLOAT2(u, v));
                        objs[objs.size() - 1].hasText = true;
                    }
                    if (tchar == 'n')
                    {
                        float xn, yn, zn;
                        file >> xn >> yn >> zn;

                        objs[objs.size() - 1].vertNorm.push_back(dx::XMFLOAT3(xn, yn, zn));
                        objs[objs.size() - 1].hasNorm = true;
                    }
                    break;


                case 'u':
                    tchar = file.get();
                    if (tchar == 's')  tchar = file.get();
                        if (tchar == 'e')  tchar = file.get();
                            if (tchar == 'm')  tchar = file.get();
                                if (tchar == 't')  tchar = file.get();
                                    if (tchar == 'l')
                                    {
                                        tchar = file.get();
                                        while (tchar != '\n')
                                        {
                                        tchar = file.get();
                                        if (tchar == '\n') continue;

                                        objs[objs.size() - 1].matName += tchar;
                                        }
                                    }
                                    break;



                case 'f':
                    tchar = file.get();

                    if (objs[objs.size() - 1].hasNorm && objs[objs.size() - 1].hasText)
                    {
                        int pi, ptc, pn;

                        for (int i = 0; i < 3; i++) {
                            file >> pi;
                            objs[objs.size() - 1].vertPosIndex.push_back(pi);
                            tchar = file.get();

                            file >> ptc;
                            objs[objs.size() - 1].vertTCIndex.push_back(ptc);
                            tchar = file.get();

                            file >> pn;
                            objs[objs.size() - 1].vertNormIndex.push_back(pn);
                            tchar = file.get();
                        }

                        if (tchar != '\n')
                        {
                            objs[objs.size() - 1].vertPosIndex.push_back(objs[objs.size() - 1].vertPosIndex[objs[objs.size() - 1].vertPosIndex.size() - 3]);
                            objs[objs.size() - 1].vertTCIndex.push_back(objs[objs.size() - 1].vertTCIndex[objs[objs.size() - 1].vertTCIndex.size() - 3]);
                            objs[objs.size() - 1].vertNormIndex.push_back(objs[objs.size() - 1].vertNormIndex[objs[objs.size() - 1].vertNormIndex.size() - 3]);

                            objs[objs.size() - 1].vertPosIndex.push_back(objs[objs.size() - 1].vertPosIndex[objs[objs.size() - 1].vertPosIndex.size() - 2]);
                            objs[objs.size() - 1].vertTCIndex.push_back(objs[objs.size() - 1].vertTCIndex[objs[objs.size() - 1].vertTCIndex.size() - 2]);
                            objs[objs.size() - 1].vertNormIndex.push_back(objs[objs.size() - 1].vertNormIndex[objs[objs.size() - 1].vertNormIndex.size() - 2]);

                            file >> pi;
                            objs[objs.size() - 1].vertPosIndex.push_back(pi);
                            tchar = file.get();

                            file >> ptc;
                            objs[objs.size() - 1].vertTCIndex.push_back(ptc);
                            tchar = file.get();

                            file >> pn;
                            objs[objs.size() - 1].vertNormIndex.push_back(pn);
                            tchar = file.get();
                        }
                    }
                    break;
                }
            }
        }
        else
        {
            return false;
        }

       

        for (int j = 0; j < objs.size(); j++)
        {
            packVertices.push_back({});
            packVertices[j].mtlName = objs[j].matName;
            for (int i = 0; i < objs[j].vertPosIndex.size(); i++)
            {
                packVertices[j].vertices.push_back({});
                packVertices[j].vertices[i].pos.x = objs[j].vertPos[objs[j].vertPosIndex[i] - 1].x;
                packVertices[j].vertices[i].pos.y = objs[j].vertPos[objs[j].vertPosIndex[i] - 1].y;
                packVertices[j].vertices[i].pos.z = objs[j].vertPos[objs[j].vertPosIndex[i] - 1].z;

                packVertices[j].vertices[i].texCoord.x = objs[j].vertTexCoord[objs[j].vertTCIndex[i] - 1].x;
                packVertices[j].vertices[i].texCoord.y = objs[j].vertTexCoord[objs[j].vertTCIndex[i] - 1].y;

                packVertices[j].vertices[i].normal.x = objs[j].vertNorm[objs[j].vertNormIndex[i] - 1].x;
                packVertices[j].vertices[i].normal.y = objs[j].vertNorm[objs[j].vertNormIndex[i] - 1].y;
                packVertices[j].vertices[i].normal.z = objs[j].vertNorm[objs[j].vertNormIndex[i] - 1].z;

                packVertices[j].vertices[i].tangent = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
            }

            for (int i = 0; i < objs[j].vertPosIndex.size(); i+=3)
            {
                const dx::XMFLOAT3& p0 = packVertices[j].vertices[i].pos;
                const dx::XMFLOAT3& p1 = packVertices[j].vertices[i + 1].pos;
                const dx::XMFLOAT3& p2 = packVertices[j].vertices[i + 2].pos;

                const dx::XMFLOAT2& tc0 = packVertices[j].vertices[i].texCoord;
                const dx::XMFLOAT2& tc1 = packVertices[j].vertices[i + 1].texCoord;
                const dx::XMFLOAT2& tc2 = packVertices[j].vertices[i + 2].texCoord;

                dx::XMFLOAT3 edge1(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
                dx::XMFLOAT3 edge2(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z);

                dx::XMFLOAT2 deltaUV1(tc1.x - tc0.x, tc1.y - tc0.y);
                dx::XMFLOAT2 deltaUV2(tc2.x - tc0.x, tc2.y - tc0.y);

                float denominator = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
                if (denominator == 0.0f)
                    continue;

                float r = 1.0f / denominator;

                dx::XMFLOAT3 tangent(
                    (edge1.x * deltaUV2.y - edge2.x * deltaUV1.y) * r,
                    (edge1.y * deltaUV2.y - edge2.y * deltaUV1.y) * r,
                    (edge1.z * deltaUV2.y - edge2.z * deltaUV1.y) * r
                );

                packVertices[j].vertices[i].tangent = dx::XMFLOAT3(packVertices[j].vertices[i].tangent.x + tangent.x,
                    packVertices[j].vertices[i].tangent.y + tangent.y,
                    packVertices[j].vertices[i].tangent.z + tangent.z);

                packVertices[j].vertices[i + 1].tangent = dx::XMFLOAT3(packVertices[j].vertices[i + 1].tangent.x + tangent.x,
                    packVertices[j].vertices[i + 1].tangent.y + tangent.y,
                    packVertices[j].vertices[i + 1].tangent.z + tangent.z);

                packVertices[j].vertices[i + 2].tangent = dx::XMFLOAT3(packVertices[j].vertices[i + 2].tangent.x + tangent.x,
                    packVertices[j].vertices[i + 2].tangent.y + tangent.y,
                    packVertices[j].vertices[i + 2].tangent.z + tangent.z);
            }

            

            packVertices[j].vectorsize = packVertices[j].vertices.size();
        }

        file.close();
        file.open(mtlfilename);

        if (file)
        {
            bool kdset = false;
            while (file)
            {
                tchar = file.get();

                switch (tchar)
                {
                case '#':
                    tchar = file.get();
                    while (tchar != '\n')
                        tchar = file.get();
                    break;

                case 'n':
                    tchar = file.get();
                    if (tchar == 'e') tchar = file.get();
                        if (tchar == 'w') tchar = file.get();
                            if (tchar == 'm') tchar = file.get();
                                if (tchar == 't') tchar = file.get();
                                    if (tchar == 'l')
                                    {
                                        tchar = file.get();
                                        while (tchar != '\n')
                                        {
                                            tchar = file.get();
                                            if (tchar == '\n') continue;

                                            lastmaterial += tchar;

                                        }
                                    }
                                    break;


                case 'K':
                    tchar = file.get();
                    if (tchar == 'd')
                    {
                        float x, y, z;

                        file >> x >> y >> z;
                        mat[lastmaterial].difColor.x = x;
                        mat[lastmaterial].difColor.y = y;
                        mat[lastmaterial].difColor.z = z;

                        kdset = true;

                    }
                    if (tchar == 'a')
                    {
                        tchar = file.get();
                        if (!kdset)
                        {
                            float x, y, z;

                            file >> x >> y >> z;

                            mat[lastmaterial].difColor.x = x;
                            mat[lastmaterial].difColor.y = y;
                            mat[lastmaterial].difColor.z = z;
                        }
                    }
                    break;

                case 'd':
                    tchar = file.get();
                    if (tchar == ' ')
                    {
                        tchar = file.get();
                        float Transparency;
                        file >> Transparency;

                        Transparency = 1.0f - Transparency;

                        mat[lastmaterial].difColor.w = Transparency;

                        if (Transparency > 0.0f)
                            mat[lastmaterial].transparent = true;
                    }
                    break;

                case 'm':
                    tchar = file.get();
                    if (tchar == 'a')
                    {
                        tchar = file.get();
                        if (tchar == 'p')
                        {
                            tchar = file.get();
                            if (tchar == '_')
                            {
                                tchar = file.get();
                                if (tchar == 'K')
                                {
                                    tchar = file.get();
                                    if (tchar == 'd')
                                    {
                                        tchar = file.get();
                                        while (tchar != '\n')
                                        {
                                            tchar = file.get();
                                            if (tchar == '\n') continue;

                                            mat[lastmaterial].textKDName += tchar;
                                        }
                                    }
                                }
                                else if (tchar == 'B')
                                {
                                    tchar = file.get();
                                    if(tchar == 'u')
                                    {
                                        tchar = file.get();
                                        if (tchar == 'm')
                                        {
                                            tchar = file.get();
                                            if (tchar == 'p')
                                            {
                                                tchar = file.get();
                                                while (tchar != '\n')
                                                {
                                                    tchar = file.get();
                                                    if (tchar == '\n') continue;

                                                    mat[lastmaterial].textBumpName += tchar;
                                                }
                                            }
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        objs.clear();
        objs.resize(0);
        return true;
	}


}






