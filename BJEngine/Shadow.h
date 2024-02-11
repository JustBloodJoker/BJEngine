#pragma once
#include "pch.h"
#include "Light.h"
#include "Shader.h"
#include "Object.h"

namespace BJEngine
{


	class Shadow
	{

	public:
		Shadow() = default;
		~Shadow() = default;

		void InitShadow(int shadowType);

		void Render(LightDesc light, std::vector<Object*> objects);

		void Close();

		ID3D11ShaderResourceView* const* GetTexture();

		dx::XMMATRIX GetView() { return camDesc[0].viewMatrix; };
		dx::XMMATRIX GetProjection() { return camDesc[0].projectionMatrix; };

	private:

		ID3D11Texture2D* textureSRV;

		void InitCubeMapShadows();
		void RenderCubeMapShadows(dx::XMFLOAT3 pos, std::vector<Object*> objects);


		void InitSimpleMapShadows();
		void RenderSimpleMapShadows(dx::XMFLOAT3  pos, dx::XMFLOAT3 dir, std::vector<Object*> objects);

		struct ConstantDepthBuffer
		{
			dx::XMMATRIX WVP;
			dx::XMFLOAT3 pos;
			float pad;
		};
		ConstantDepthBuffer depthBuffer;
		ID3D11Buffer* pDepthConstantBuffer;

		ID3D11ShaderResourceView* srv;
		ID3D11DepthStencilView* dsv[6];

		CameraDesc camDesc[6];

		Shader* shader;

		D3D11_VIEWPORT vp = {};
	};


}