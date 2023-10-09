#pragma once
#include "pch.h"
#include "Object.h"

namespace BJEngine {

	struct LightDesc {
		LightDesc()
		{
			ZeroMemory(this, sizeof(LightDesc));
			color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
		}

		dx::XMFLOAT4 pos;
		dx::XMFLOAT4 dir;
		dx::XMFLOAT4 color;

		dx::XMFLOAT3 att;
		float angle;

		int lightType;
		int enabled;
		int pad;
		int pad1;
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

		bool InitLight(ID3D11Device* pd3dDevice);
		void DrawLight(ID3D11DeviceContext* pImmediateContext);

		void SetLightDesc(LightDesc* tld);
		void SetPos(float x, float y, float z, int index);
		
		dx::XMFLOAT3 GetPos(int index);
		dx::XMFLOAT3 GetDir(int index);

		void InitShadowMap();

		LightDesc GetDesc(int index);

	private:
		ID3D11Buffer* lightBuffer;
		ConstantBufferLight bufferdesclight;

		dx::XMMATRIX viewSM;
		dx::XMMATRIX projectionSM;

	};


}