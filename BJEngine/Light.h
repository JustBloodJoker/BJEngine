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
			shadowEnabled = 1;
		}

		dx::XMFLOAT4 pos;
		dx::XMFLOAT4 dir;
		dx::XMFLOAT4 color;

		dx::XMFLOAT3 att;
		float angle;

		int lightType;
		int enabled;
		int shadowEnabled;
		int pad1;

	};

	class Shadow
	{
		

	public:

		
		virtual void Draw() = 0;

		virtual void GenerateView(const LightDesc ld) = 0;

		virtual void BindSRV(int deltIndex) = 0;

		virtual void Close() = 0;

	protected:

		virtual void Init() = 0;

		bool isInited;

		Shader* shader;
		D3D11_VIEWPORT vp;
		DepthStencil* depthStencil;
		
	};

	class OmnidirectionalShadow
		: public Shadow
	{
		struct OmnidirectionalShadowConstantBuffer
		{
			dx::XMMATRIX shViewProjection[6];
		};
		static ID3D11Buffer* shadowCBuffer;

		OmnidirectionalShadowConstantBuffer matrices;

		void Init() override;

	public:

		OmnidirectionalShadow();

		
		void Draw() override;

		void GenerateView(const LightDesc ld) override;

		void BindSRV(int deltIndex) override;

		void Close() override;


	};

	class SimpleShadow
		: public Shadow
	{
		struct SimpleShadowConstantBuffer
		{
			dx::XMMATRIX shViewProjection;
		};
		static ID3D11Buffer* shadowCBuffer;

		SimpleShadowConstantBuffer matrices;

		void Init() override;

	public:

		SimpleShadow();


		void Draw() override;

		void GenerateView(const LightDesc ld) override;

		void BindSRV(int deltIndex) override;

		void Close() override;

	};

}