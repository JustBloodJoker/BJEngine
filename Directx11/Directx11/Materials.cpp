#include "pch.h"
#include "Materials.h"

#include "Object.h"
namespace BJEngine
{

	BJEngine::Materials::Materials(ID3D11Device* pd3dDevice)
	{
		Init(pd3dDevice);
	}

	void BJEngine::Materials::Draw(ID3D11DeviceContext* pImmediateContext, int registerMaterialPos, 
		int registerTexturePos, int registerNormalTexturePos, int registerRoughnessTexturePos)
	{
		if (isInit)
		{

			pImmediateContext->UpdateSubresource(pMaterialBuffer, 0, NULL, &cmbDesc, 0, 0);
			pImmediateContext->PSSetConstantBuffers(registerMaterialPos, 1, &pMaterialBuffer);

			if (cmbDesc.matDesc.ishavealphablend)
				Blend::Get()->DrawAlphaBlend(pImmediateContext);
			else
				Blend::Get()->DrawNoBlend(pImmediateContext);

			if (cmbDesc.matDesc.isTexture)
			{
				pImmediateContext->GenerateMips(texture->GetTexture());
				pImmediateContext->PSSetShaderResources(registerTexturePos, 1, &texture->GetTexture());
				pImmediateContext->PSSetSamplers(registerTexturePos, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isNormalTexture)
			{
				pImmediateContext->GenerateMips(normalTexture->GetTexture());
				pImmediateContext->PSSetShaderResources(registerNormalTexturePos, 1, &normalTexture->GetTexture());
				pImmediateContext->PSSetSamplers(registerTexturePos, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isRoughnessTexture)
			{
				pImmediateContext->GenerateMips(roughnessTexture->GetTexture());
				pImmediateContext->PSSetShaderResources(registerRoughnessTexturePos, 1, &roughnessTexture->GetTexture());
				pImmediateContext->PSSetSamplers(registerTexturePos, 1, Textures::GetWrapState());
			}

		}
	}

	void BJEngine::Materials::Init(ID3D11Device* pd3dDevice)
	{
		pMaterialBuffer = Object::InitConstantBuffer<ConstantMaterialBuffer>(pd3dDevice);

		if (!pMaterialBuffer)
		{
			Log::Get()->Err("Error in initialize material constant buffer");
			isInit = false;
			return;
		}
		isInit = true;
	}

	void Materials::HasAlphaChannel(ID3D11ShaderResourceView* textureSRV)
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

	void Materials::SetParam(short paramType, dx::XMFLOAT4 param)
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
		}
	}

	void Materials::SetParam(short paramType, float param)
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

	void Materials::SetTexture(short textureType, std::wstring textureName, ID3D11Device* pd3dDevice)
	{
		switch (textureType)
		{
		case HAS_TEXTURE:
			
			texture = new Textures(textureName.c_str());
			texture->InitTextures(pd3dDevice);
			cmbDesc.matDesc.isTexture = true;

			HasAlphaChannel(texture->GetTexture());

			break;
		case HAS_NORMAL_TEXTURE:

			normalTexture = new Textures(textureName.c_str());
			normalTexture->InitTextures(pd3dDevice);
			cmbDesc.matDesc.isNormalTexture = true;
		
			break;

		case HAS_ROUGHNESS_TEXTURE:

			roughnessTexture = new Textures(textureName.c_str());
			roughnessTexture->InitTextures(pd3dDevice);
			cmbDesc.matDesc.isRoughnessTexture = true;
			
			break;

		default:
			
			Log::Get()->Err("incorrect texture input type");
			
			break;
		}
	}

	void Materials::SetTexture(short textureType, Textures* ttexture, ID3D11Device* pd3dDevice)
	{
		switch (textureType)
		{
		case HAS_TEXTURE:

			this->texture = ttexture;
			texture->InitTextures(pd3dDevice);
			cmbDesc.matDesc.isTexture = true;

			break;
		case HAS_NORMAL_TEXTURE:

			this->normalTexture = ttexture;
			normalTexture->InitTextures(pd3dDevice);
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
		RELEASE(pMaterialBuffer);

	}

}