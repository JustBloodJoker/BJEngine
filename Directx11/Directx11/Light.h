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
		int typeofLight;
		dx::XMFLOAT4 ambient;
		dx::XMFLOAT4 diffuse;
		
		
	};


	class Light
	{
	private:

		friend class DirectionalLight;
		friend class PointLight;
		friend class SpotLight;

		struct ConstantBufferLight
		{
			LightDesc light[5];
			int lightsCount;
			int pad;
			int pad1;
			int pad2;
		};

	public:

		Light();
		Light(LightDesc* tld);
		~Light();

		void Close();

		ID3D11Buffer* GetConstantBuffer();
		bool IsLightOn();

		bool InitLight(ID3D11Device* pd3dDevice);
		void DrawLight(ID3D11DeviceContext* pImmediateContext);

		void SetLightDesc(LightDesc* tld);
		void SetPos(float x, float y, float z, int index);
		dx::XMFLOAT3 GetPos(int index);

		
	private:
		ID3D11Buffer* lightBuffer;
		ConstantBufferLight bufferdesclight;

		dx::XMMATRIX view;
		dx::XMMATRIX projection;

	};


}