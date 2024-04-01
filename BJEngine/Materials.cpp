#include "Materials.h"
#include "Blend.h"

namespace BJEngine
{
	std::vector<Materials*> Materials::materialVector;
	ID3D11Buffer* Materials::pMaterialBuffer = nullptr;

	Materials::Materials()
	{
		Init();
		name = std::string("MATERIAL") + std::to_string(materialVector.size());
		materialVector.push_back((this));
	}

	void Materials::Draw()
	{
		if (isInit)
		{
			GP::GetDeviceContext()->UpdateSubresource(pMaterialBuffer, 0, NULL, &cmbDesc, 0, 0);
			

			if (cmbDesc.matDesc.ishaveTransparency)
				Blend::Get()->DrawTransparencyBlend();
			else if (cmbDesc.matDesc.ishavealphablend)
				Blend::Get()->DrawAlphaBlend();
			else
				Blend::Get()->DrawNoBlend();
			        

			

			if (cmbDesc.matDesc.isTexture)
			{
				GP::GetDeviceContext()->GenerateMips(textures[HAS_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(DIFFUSE_TEXTURE_POS, 1, &textures[HAS_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(DIFFUSE_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isNormalTexture)
			{
				GP::GetDeviceContext()->GenerateMips(textures[HAS_NORMAL_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(NORMAL_TEXTURE_POS, 1, &textures[HAS_NORMAL_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(NORMAL_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isRoughnessTexture)
			{
				GP::GetDeviceContext()->GenerateMips(textures[HAS_ROUGHNESS_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(ROUGHNESS_TEXTURE_POS, 1, &textures[HAS_ROUGHNESS_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(ROUGHNESS_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isEmissionTexture)
			{
				GP::GetDeviceContext()->GenerateMips(textures[HAS_EMISSION_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(EMISSION_TEXTURE_POS, 1, &textures[HAS_EMISSION_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(EMISSION_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}

			if (cmbDesc.matDesc.isSpecularTexture)
			{
				GP::GetDeviceContext()->GenerateMips(textures[HAS_SPECULAR_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetShaderResources(SPECULAR_TEXTURE_POS, 1, &textures[HAS_SPECULAR_TEXTURE]->GetTexture());
				GP::GetDeviceContext()->PSSetSamplers(SPECULAR_SAMPLERSTATE_POS, 1, Textures::GetWrapState());
			}
		}
	}

	
	int Materials::GenRenderPriority()
	{

		if (cmbDesc.matDesc.ishaveTransparency)
			return 2;

		return 1;
	}

	std::string Materials::GetName() const
	{
		return name;
	}

	const std::string Materials::GetTexturePath(MATERIAL_TYPE textureType)
	{
		if (textures[textureType] != nullptr)
		{
			std::wstring tmp = textures[textureType]->GetTexturePath();
			
			return std::string(tmp.begin(), tmp.end());

		}
		else
			return std::string();
	}

	void Materials::Init()
	{
		if(pMaterialBuffer == nullptr)
			pMaterialBuffer = Helper::InitConstantBuffer<BJEStruct::ConstantMaterialBuffer>(GP::GetDevice());

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

			case OPACITY:
				cmbDesc.matDesc.diffuse.w = param;
				if (param < 1.0f)
				{
					cmbDesc.matDesc.ishaveTransparency = true;
				}
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

			textures[HAS_TEXTURE] = new Textures(textureName.c_str());
			textures[HAS_TEXTURE]->InitTextures();
			cmbDesc.matDesc.isTexture = true;

			HasAlphaChannel(textures[HAS_TEXTURE]->GetTexture());

			break;
		case HAS_NORMAL_TEXTURE:

			textures[HAS_NORMAL_TEXTURE] = new Textures(textureName.c_str());
			textures[HAS_NORMAL_TEXTURE]->InitTextures();

			cmbDesc.matDesc.isNormalTexture = true;

			break;

		case HAS_ROUGHNESS_TEXTURE:

			textures[HAS_ROUGHNESS_TEXTURE] = new Textures(textureName.c_str());
			textures[HAS_ROUGHNESS_TEXTURE]->InitTextures();
			cmbDesc.matDesc.isRoughnessTexture = true;

			break;

		case HAS_EMISSION_TEXTURE:
			
			textures[HAS_EMISSION_TEXTURE] = new Textures(textureName.c_str());
			textures[HAS_EMISSION_TEXTURE]->InitTextures();
			cmbDesc.matDesc.isEmissionTexture = true;

			break;

		case HAS_SPECULAR_TEXTURE:

			textures[HAS_SPECULAR_TEXTURE] = new Textures(textureName.c_str());
			textures[HAS_SPECULAR_TEXTURE]->InitTextures();
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

			textures[HAS_TEXTURE] = ttexture;
			textures[HAS_TEXTURE]->InitTextures();
			cmbDesc.matDesc.isTexture = true;

			break;
		case HAS_NORMAL_TEXTURE:

			textures[HAS_NORMAL_TEXTURE] = ttexture;
			textures[HAS_NORMAL_TEXTURE]->InitTextures();
			cmbDesc.matDesc.isNormalTexture = true;

			break;
		default:

			Log::Get()->Err("incorrect texture input type");

			break;
		}
	}

	void  Materials::Close()
	{
		for (auto& el : textures)
			CLOSE(el.second);

		
		RELEASE(pMaterialBuffer);
	}

}