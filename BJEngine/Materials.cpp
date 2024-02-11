#include "Materials.h"
#include "Blend.h"

namespace BJEngine
{
	std::vector<BJEngine::Object*>* Materials::objects = nullptr;
	CD3D11_VIEWPORT Materials::reflect_vp;

	Materials::Materials()
	{
		Init();
		
		////////////////////////
		//	reflectionTexture = new RenderTarget(512, 512, D3D11_SRV_DIMENSION_TEXTURECUBE, GP::pDevice, GP::GetDeviceContext());
		////////////////////////
	}

	void Materials::Draw(int registerMaterialPos)
	{
		if (isInit)
		{
			GP::GetDeviceContext()->UpdateSubresource(pMaterialBuffer, 0, NULL, &cmbDesc, 0, 0);
			GP::GetDeviceContext()->PSSetConstantBuffers(registerMaterialPos, 1, &pMaterialBuffer);

			if (cmbDesc.matDesc.ishaveTransparency)
				Blend::Get()->DrawTransparencyBlend();

			else if (cmbDesc.matDesc.ishavealphablend)
				Blend::Get()->DrawAlphaBlend();
			else
				Blend::Get()->DrawNoBlend();

			

			if (cmbDesc.matDesc.isTexture)
			{
				GP::GetDeviceContext()->GenerateMips(texture->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(DIFFUSE_TEXTURE_POS, 1, &texture->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(DIFFUSE_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isNormalTexture)
			{
				GP::GetDeviceContext()->GenerateMips(normalTexture->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(NORMAL_TEXTURE_POS, 1, &normalTexture->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(NORMAL_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isRoughnessTexture)
			{
				GP::GetDeviceContext()->GenerateMips(roughnessTexture->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(ROUGHNESS_TEXTURE_POS, 1, &roughnessTexture->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(ROUGHNESS_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isEmissionTexture)
			{
				GP::GetDeviceContext()->GenerateMips(emissionTexture->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(EMISSION_TEXTURE_POS, 1, &emissionTexture->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(EMISSION_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isSpecularTexture)
			{
				GP::GetDeviceContext()->GenerateMips(specularTexture->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(SPECULAR_TEXTURE_POS, 1, &specularTexture->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(SPECULAR_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

		}
	}

	void Materials::Draw(int registerMaterialPos, Element* elem)
	{
		if (isInit)
		{
			//GenerateReflectionTexture(elem->GetWorldPosition());

			if (isReflect)
			{
				
				

				return;
			}
			else
			{
				Draw(registerMaterialPos);
			}
		}
	}

	void Materials::Init()
	{
		pMaterialBuffer = Helper::InitConstantBuffer<ConstantMaterialBuffer>(GP::GetDevice());

		if (!pMaterialBuffer)
		{
			Log::Get()->Err("Error in initialize material constant buffer");
			isInit = false;
			return;
		}

		Materials::reflect_vp.Height = 512;
		Materials::reflect_vp.Width = 512;
		Materials::reflect_vp.MaxDepth = 1.0f;
		Materials::reflect_vp.MinDepth = 0.0f;
		Materials::reflect_vp.TopLeftX = 0.0f;
		Materials::reflect_vp.TopLeftY = 0.0f;

		isInit = true;
	}

	void Materials::GenerateReflectionTexture(dx::XMFLOAT3 pos)
	{
		//CameraDesc camDesc[6];

		//camDesc[0].projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);
		//camDesc[1].projectionMatrix = camDesc[0].projectionMatrix;
		//camDesc[2].projectionMatrix = camDesc[0].projectionMatrix;
		//camDesc[3].projectionMatrix = camDesc[0].projectionMatrix;
		//camDesc[4].projectionMatrix = camDesc[0].projectionMatrix;
		//camDesc[5].projectionMatrix = camDesc[0].projectionMatrix;

		//dx::XMFLOAT4 forward = dx::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		//dx::XMFLOAT4 up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		//camDesc[0].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
		//	dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
		//	dx::XMLoadFloat4(&up));

		//forward = dx::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
		//up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		//camDesc[1].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
		//	dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
		//	dx::XMLoadFloat4(&up));

		//forward = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		//up = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);

		//camDesc[2].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
		//	dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
		//	dx::XMLoadFloat4(&up));

		//forward = dx::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		//up = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

		//camDesc[3].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
		//	dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
		//	dx::XMLoadFloat4(&up));

		//forward = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
		//up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		//camDesc[4].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
		//	dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
		//	dx::XMLoadFloat4(&up));

		//forward = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
		//up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		//camDesc[5].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
		//	dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
		//	dx::XMLoadFloat4(&up));

		//
		//GP::GetDeviceContext()->RSGetViewports(&sizeViewPorts, &curr_vp);

		//GP::GetDeviceContext()->RSSetViewports(1, &reflect_vp);



		//for (int x = 0; x < 6; x++)
		//{
		//	camDesc[x].GenFrustum();

		//	reflectionTexture->Clear(x);
		//	reflectionTexture->Bind(x);

		//	for (auto& el : *objects)
		//	{
		//		if (el && el->IsInited())
		//		{
		//			el->Draw(camDesc[x]);
		//		}
		//	}

		//	if(Input::Get()->CheckKeyState(DIK_6))
		//		reflectionTexture->SaveRTVTexture(L"3.png");
		//
		//}

		//GP::GetDeviceContext()->RSSetViewports(1, &curr_vp);
		//RenderTarget::sceneRTV->Bind();
	
	}

	void Materials::HasAlphaChannel(ID3D11ShaderResourceView* textureSRV)
	{
		if (textureSRV)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			textureSRV->GetDesc(&srvDesc);

			switch (srvDesc.Format)
			{
			case DXGI_FORMAT_R8G8B8A8_UNORM:
			case DXGI_FORMAT_R8G8B8A8_SNORM:
			case DXGI_FORMAT_B8G8R8A8_UNORM:
			case DXGI_FORMAT_R16G16B16A16_FLOAT:
			case DXGI_FORMAT_R16G16B16A16_UNORM:
			case DXGI_FORMAT_R16G16B16A16_SNORM:
			case DXGI_FORMAT_R32G32B32A32_FLOAT:
			case DXGI_FORMAT_R32G32B32A32_UINT:
			case DXGI_FORMAT_R32G32B32A32_SINT:
				cmbDesc.matDesc.ishavealphablend = true;
			default:
				cmbDesc.matDesc.ishavealphablend = false;
			}
		}
		else
		{
			cmbDesc.matDesc.ishavealphablend = false;
		}

	}

	void Materials::SetParam(MATERIAL_TYPE paramType, dx::XMFLOAT4 param)
	{
		if (isInit)
		{
			switch (paramType)
			{
			case AMBIENT:
				cmbDesc.matDesc.ambient = param;
				break;
			case DIFFUSE:
				cmbDesc.matDesc.diffuse = param;
				break;
			case EMISSIVE:
				cmbDesc.matDesc.emissive = param;
				break;
			case SPECULAR:
				cmbDesc.matDesc.specular = param;
				break;
			default:
				Log::Get()->Err("Incorrect param type");
				break;
			}

			if (param.w < 1.0f)
			{
				cmbDesc.matDesc.ishaveTransparency = true;
			}

		}
	}

	void Materials::SetParam(MATERIAL_TYPE paramType, float param)
	{
		if (isInit)
		{
			switch (paramType)
			{
			case SPECULAR_POWER:
				cmbDesc.matDesc.specularPower = param;
				break;

			

			default:
				Log::Get()->Err("Incorrect param type");
				break;
			}
		}
	}

	void Materials::SetTexture(MATERIAL_TYPE textureType, std::wstring textureName)
	{
		switch (textureType)
		{
		case HAS_TEXTURE:

			texture = new Textures(textureName.c_str());
			texture->InitTextures();
			cmbDesc.matDesc.isTexture = true;

			HasAlphaChannel(texture->GetTexture());

			break;
		case HAS_NORMAL_TEXTURE:

			normalTexture = new Textures(textureName.c_str());
			normalTexture->InitTextures();
			cmbDesc.matDesc.isNormalTexture = true;

			break;

		case HAS_ROUGHNESS_TEXTURE:

			roughnessTexture = new Textures(textureName.c_str());
			roughnessTexture->InitTextures();
			cmbDesc.matDesc.isRoughnessTexture = true;

			break;

		case HAS_EMISSION_TEXTURE:
			
			emissionTexture = new Textures(textureName.c_str());
			emissionTexture->InitTextures();
			cmbDesc.matDesc.isEmissionTexture = true;

			break;

		case HAS_SPECULAR_TEXTURE:

			specularTexture = new Textures(textureName.c_str());
			specularTexture->InitTextures();
			cmbDesc.matDesc.isSpecularTexture = true;

			break;

		case HAS_REFLECTION:
			
			isReflect = true;

			break;

		default:

			Log::Get()->Err("incorrect texture input type");

			break;
		}
	}

	void Materials::SetTexture(MATERIAL_TYPE textureType, Textures* ttexture)
	{
		switch (textureType)
		{
		case HAS_TEXTURE:

			this->texture = ttexture;
			texture->InitTextures();
			cmbDesc.matDesc.isTexture = true;

			break;
		case HAS_NORMAL_TEXTURE:

			this->normalTexture = ttexture;
			normalTexture->InitTextures();
			cmbDesc.matDesc.isNormalTexture = true;

			break;
		default:

			Log::Get()->Err("incorrect texture input type");

			break;
		}
	}

	void  Materials::Close()
	{
		CLOSE(texture);
		CLOSE(normalTexture);
		CLOSE(roughnessTexture);
		CLOSE(emissionTexture);
		CLOSE(specularTexture);
		RELEASE(pMaterialBuffer);
	}

}