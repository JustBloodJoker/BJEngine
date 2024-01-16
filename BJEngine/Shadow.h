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

		void InitShadow(ID3D11Device* pd3dDevice, int shadowType);

		void Render(ID3D11DeviceContext* pImmediateContext, LightDesc light, std::vector<Object*> objects);

		void Close();

		ID3D11ShaderResourceView* const* GetTexture();

		dx::XMMATRIX GetView() { return vw; };
		dx::XMMATRIX GetProjection() { return prj; };

	private:

		void InitCubeMapShadows(ID3D11Device* pd3dDevice);
		void RenderCubeMapShadows(ID3D11DeviceContext* pImmediateContext, dx::XMFLOAT3 pos, std::vector<Object*> objects);


		void InitSimpleMapShadows(ID3D11Device* pd3dDevice);
		void RenderSimpleMapShadows(ID3D11DeviceContext* pImmediateContext, dx::XMFLOAT3  pos, dx::XMFLOAT3 dir, std::vector<Object*> objects);

		struct ConstantDepthBuffer
		{
			dx::XMMATRIX WVP;
		};
		ConstantDepthBuffer depthBuffer;
		ID3D11Buffer* pDepthConstantBuffer;

		ID3D11ShaderResourceView* srv;
		ID3D11DepthStencilView* dsv[6];

		dx::XMMATRIX vw;
		dx::XMMATRIX prj;

		Shader* shader;

		D3D11_VIEWPORT vp = {};
	};


}