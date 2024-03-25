#pragma once 
#include "pch.h"

namespace BJEngine {


	class Textures
	{
		static size_t count;

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
		static ID3D11SamplerState* const* GetClampState();


		ID3D11ShaderResourceView*& GetTexture() { return Texture; }

		std::wstring GetTexturePath() const
		{
			return TextureName;
		}

	protected:

		ID3D11ShaderResourceView* Texture;
		std::wstring TextureName;

	private:
		
		static ID3D11SamplerState* WrapState;
		static ID3D11SamplerState* BorderState;
		static ID3D11SamplerState* ClampState;
		static bool deletedStatic;
		static D3D11_FILTER shadowFilter;
		static D3D11_FILTER textureFilter;

	};


}