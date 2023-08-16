#pragma once
#include "pch.h"

namespace BJEngine {


	class Textures
	{
	public:
		Textures(const wchar_t* TextureName);
		~Textures();
		
		void Close();

		bool InitTextures(ID3D11Device* device);
		bool InitCubeMap(ID3D11Device* device);

		ID3D11ShaderResourceView*& GetTexture() { return Texture; }
		ID3D11SamplerState*& GetTexSamplerState() { return TexSamplerState; }
	private:
		ID3D11ShaderResourceView* Texture;
		ID3D11SamplerState* TexSamplerState;

		const wchar_t* TextureName;
	};


}