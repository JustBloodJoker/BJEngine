#pragma once
#include "pch.h"

namespace BJEngine {


	class Textures
	{
	public:
		Textures(const wchar_t* TextureName);
		Textures(std::wstring TextureName);
		~Textures();

		void Close();

		bool InitTextures(ID3D11Device* device);
		bool InitCubeMap(ID3D11Device* device);

		static bool InitStates(ID3D11Device* pd3dDevice);
		static ID3D11SamplerState* const* GetBorderState();
		static ID3D11SamplerState* const* GetWrapState();


		ID3D11ShaderResourceView*& GetTexture() { return Texture; }
	private:
		ID3D11ShaderResourceView* Texture;

		static ID3D11SamplerState* WrapState;
		static ID3D11SamplerState* BorderState;
		static bool deletedStatic;
		static D3D11_FILTER shadowFilter;
		static D3D11_FILTER textureFilter;

		std::wstring TextureName;
	};


}