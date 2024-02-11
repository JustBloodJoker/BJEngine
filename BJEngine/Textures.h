#pragma once 
#include "pch.h"

namespace BJEngine {


	class Textures
	{
	public:
		Textures() = default;
		Textures(const wchar_t* TextureName);
		Textures(std::wstring TextureName);
		virtual ~Textures();

		virtual void Close();

		virtual bool InitTextures();
		bool InitCubeMap();

		static bool InitStates();
		static ID3D11SamplerState* const* GetBorderState();
		static ID3D11SamplerState* const* GetWrapState();


		ID3D11ShaderResourceView*& GetTexture() { return Texture; }

	protected:

		ID3D11ShaderResourceView* Texture;
		std::wstring TextureName;

	private:
		
		static ID3D11SamplerState* WrapState;
		static ID3D11SamplerState* BorderState;
		static bool deletedStatic;
		static D3D11_FILTER shadowFilter;
		static D3D11_FILTER textureFilter;

	};


}