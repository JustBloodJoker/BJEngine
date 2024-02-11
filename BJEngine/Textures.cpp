#include "Textures.h"

namespace BJEngine {

	bool Textures::deletedStatic = true;
	ID3D11SamplerState* Textures::WrapState = {};
	ID3D11SamplerState* Textures::BorderState = {};

	D3D11_FILTER Textures::textureFilter = D3D11_FILTER_ANISOTROPIC;
	D3D11_FILTER Textures::shadowFilter = D3D11_FILTER_ANISOTROPIC;

	Textures::Textures(const wchar_t* TextureName) :
		TextureName(TextureName)
	{
		this->Texture = nullptr;
	}

	Textures::Textures(std::wstring TextureName) :
		TextureName(TextureName)
	{
		this->Texture = nullptr;
	}

	Textures::~Textures()
	{

	}

	void Textures::Close()
	{
		if (!deletedStatic)
		{
			RELEASE(BorderState);
			RELEASE(WrapState);
		}
		RELEASE(Texture);
	}

	bool Textures::InitTextures()
	{
		HRESULT hr = S_OK;

		hr = D3DX11CreateShaderResourceViewFromFile(GP::GetDevice(), TextureName.c_str(),
			NULL, NULL, &Texture, NULL);

		if (FAILED(hr)) {
			Log::Get()->Err("ERROR IN CREATING TEXTURE");
			return true;
		}

		if (deletedStatic)
		{
			Textures::InitStates();
		}

		return false;


	}

	bool Textures::InitCubeMap()
	{
		D3DX11_IMAGE_LOAD_INFO loadInfo;
		loadInfo.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		ID3D11Texture2D* TTexture = 0;
		HRESULT hr = D3DX11CreateTextureFromFile(GP::GetDevice(), TextureName.c_str(),
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

		hr = GP::GetDevice()->CreateShaderResourceView(TTexture, &SMViewDesc, &Texture);
		if (FAILED(hr)) {
			Log::Get()->Err("CreateShaderResourseView error");
			return true;
		}

		if (deletedStatic)
		{
			Textures::InitStates();
		}

		return false;
	}

	bool Textures::InitStates()
	{
		HRESULT hr = S_OK;

		RELEASE(BorderState);
		RELEASE(WrapState);

		D3D11_SAMPLER_DESC sampDesc;
		ZeroMemory(&sampDesc, sizeof(sampDesc));
		sampDesc.Filter = textureFilter;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
		sampDesc.MipLODBias = 0;
		sampDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		sampDesc.MinLOD = 0;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;

		hr = GP::GetDevice()->CreateSamplerState(&sampDesc, &WrapState);
		if (FAILED(hr)) {
			Log::Get()->Err("CreateSamplerState error");
			return true;
		}

		ZeroMemory(&sampDesc, sizeof(D3D11_SAMPLER_DESC));
		sampDesc.Filter = shadowFilter;
		sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		sampDesc.BorderColor[0] = 1.0f;
		sampDesc.BorderColor[1] = 1.0f;
		sampDesc.BorderColor[2] = 1.0f;
		sampDesc.BorderColor[3] = 1.0f;
		sampDesc.MinLOD = 0.f;
		sampDesc.MaxLOD = D3D11_FLOAT32_MAX;
		sampDesc.MipLODBias = 0.f; 
		sampDesc.MaxAnisotropy = D3D11_DEFAULT_MAX_ANISOTROPY;
		sampDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;
		sampDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		GP::GetDevice()->CreateSamplerState(
			&sampDesc,
			&BorderState
		);

		deletedStatic = false;

		return true;
	}


	ID3D11SamplerState* const* Textures::GetBorderState()
	{
		return &BorderState;
	}

	ID3D11SamplerState* const* Textures::GetWrapState()
	{
		return &WrapState;
	}


}