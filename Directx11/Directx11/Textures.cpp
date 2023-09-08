#include "pch.h"
#include "Textures.h"

namespace BJEngine {


	Textures::Textures(const wchar_t* TextureName) :
		TextureName(TextureName)
	{
		this->TexSamplerState = nullptr;
		this->Texture = nullptr;
	}

	Textures::~Textures()
	{
		
	}

	void Textures::Close()
	{
		RELEASE(TexSamplerState);
		RELEASE(Texture);
	}

	bool Textures::InitTextures(ID3D11Device* device)
	{
		HRESULT hr = S_OK;

		hr = D3DX11CreateShaderResourceViewFromFile(device, TextureName,
			NULL, NULL, &Texture, NULL);

		if (FAILED(hr)) {
			Log::Get()->Err("ERROR IN CREATING TEXTURE");
			return true;
		}

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampDesc, &TexSamplerState);
		
		if (FAILED(hr)) {
			return true;
		}

		return false;
	}

	bool Textures::InitCubeMap(ID3D11Device* device)
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		ID3D11Texture2D* TTexture = 0;
		HRESULT hr = D3DX11CreateTextureFromFile(device, TextureName,
			&loadInfo, 0, (ID3D11Resource**)&TTexture, 0);

		if (FAILED(hr)) {
			Log::Get()->Err("CreateTexture error");
			return true;
		}

		D3D11_TEXTURE2D_DESC TextureDesc;
		TTexture->GetDesc(&TextureDesc);

		D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
		SMViewDesc.Format = TextureDesc.Format;
		SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		SMViewDesc.TextureCube.MipLevels = TextureDesc.MipLevels;
		SMViewDesc.TextureCube.MostDetailedMip = 0;

		hr = device->CreateShaderResourceView(TTexture, &SMViewDesc, &Texture);
		if (FAILED(hr)) {
			Log::Get()->Err("CreateShaderResourseView error");
			return true;
		}

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		hr = device->CreateSamplerState(&sampDesc, &TexSamplerState);
		if (FAILED(hr)) {
			Log::Get()->Err("CreateSamplerState error");
			return true;
		}

		return false;
	}


}