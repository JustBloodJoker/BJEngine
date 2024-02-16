#pragma once
#include "pch.h"
#include "Shader.h"

namespace BJEngine {

	struct LightDesc 
	{
		
		LightDesc()
		{
			ZeroMemory(this, sizeof(LightDesc));
			color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
			dir = dx::XMFLOAT4(-1.0f, 0.0f, -1.0f, 1.0f);
			pos = dx::XMFLOAT4(0.0f, 500.0f, 0.0f, 1.0f);
			att = dx::XMFLOAT3(0.01f, 0.02f, 0.0f);
			angle = M_PI / 8;
			enabled = 1;
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

	class Shadow
	{
		struct ShadowConstantBuffer
		{
			dx::XMMATRIX shViewProjection[6];
		};
		static ID3D11Buffer* shadowCBuffer;

	public:

		Shadow() = default;
		Shadow(short type);

		void Init(short type);
		void Draw();

		void GenerateView(const LightDesc ld);

		void BindSRV(int deltIndex);

		void Close();
	private:

		

		bool isInited;
		short shadowType;

		Shader* shader;

		D3D11_VIEWPORT vp;

		DepthStencil* depthStencil;
		ShadowConstantBuffer matrices;
	};


}