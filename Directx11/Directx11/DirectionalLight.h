#pragma once
#include "pch.h"
#include "Light.h"

namespace BJEngine {


	struct DirectionalLightDesc
	{
		DirectionalLightDesc()
		{
			ZeroMemory(this, sizeof(DirectionalLightDesc));
		}

		dx::XMFLOAT3 dir;
		float pad;
		dx::XMFLOAT4 ambient;
		dx::XMFLOAT4 diffuse;
	};

	class DirectionalLight : public Light

	{
	public:

		DirectionalLight();
		DirectionalLight(DirectionalLightDesc* light);
		~DirectionalLight();

		bool InitLight(ID3D11Device* pd3dDevice) override;
		void DrawLight(ID3D11DeviceContext* pImmediateContext) override;

	private:

		DirectionalLightDesc* light;

	};


}