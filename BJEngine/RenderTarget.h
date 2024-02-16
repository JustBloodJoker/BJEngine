#pragma once

#include "pch.h"
#include "Shader.h"
#include "Textures.h"

namespace BJEngine
{


	class RenderTarget
	{
		
		static unsigned int size;

		static class Screen
		{
		public:
			
			static void Init();
			static void Draw(ID3D11ShaderResourceView* srv, BJEUtils::POST_PROCESSING type);

			static void Close();

		private:

			static Shader* GetShader(BJEUtils::POST_PROCESSING type);

			static BJEStruct::VertexBackGround vertices[4];


			static ID3D11Buffer* pVertexBuffer;
			static Shader* shader;
			static Shader* inverseShader;
			static Shader* greyShader;
			static Shader* sharpnessShader;
			static Shader* simpleBlurShader;
			static Shader* boundaryDelineationShader;

			static bool isInited;
		};


	public:
		
		RenderTarget() = default;
		RenderTarget(ID3D11Texture2D* texture, bool MSAA = false);
		RenderTarget(int width, int height, bool MSAA = false);

		ID3D11RenderTargetView*& GetRTV();

		void ClearRTV();

		void CreateCopyTexture();
		ID3D11ShaderResourceView* GetCopyTexture();

		void Close();

		ID3D11ShaderResourceView* GetSRV();
		ID3D11Texture2D* GetTexture();

		void SaveRTVTexture(std::wstring&& fileName);

		void DrawTexture(ID3D11ShaderResourceView* srv, BJEUtils::POST_PROCESSING type);

	private:

		

		
		ID3D11RenderTargetView* renderTargetView;

		float clearColor[4] = { 0.1f,0.1f,0.1f,1.0f };

		ID3D11Texture2D* renderTargetTexture;
		ID3D11Texture2D* tempRenderTargetTexture;

		ID3D11ShaderResourceView* tempShaderResourceView;
		ID3D11ShaderResourceView* shaderResourceView;

	};



}