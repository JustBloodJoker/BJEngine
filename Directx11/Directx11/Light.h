#pragma once
#include "pch.h"

namespace BJEngine {

	struct LightDesc {
		LightDesc()
		{
			ZeroMemory(this, sizeof(LightDesc));
		}
		dx::XMFLOAT3 pos;
		float range;
		dx::XMFLOAT3 dir;
		float cone;
		dx::XMFLOAT3 att;
		float pad;
		dx::XMFLOAT4 ambient;
		dx::XMFLOAT4 diffuse;
		
		
	};


	class Light
	{
	private:

		friend class DirectionalLight;
		friend class PointLight;
		friend class SpotLight;

	public:

		Light();
		~Light();

		void Close();

		ID3D11Buffer* GetConstantBuffer();
		bool IsLightOn();

		virtual bool InitLight(ID3D11Device* pd3dDevice);
		virtual void DrawLight(ID3D11DeviceContext* pImmediateContext);

	private:

		ID3D11Buffer* lightBuffer;
		bool isLightOn = false;

		struct ConstantBufferLight
		{
			LightDesc light;
		};
	};


}