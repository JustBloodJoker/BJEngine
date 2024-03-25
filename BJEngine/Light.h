#pragma once
#include "pch.h"
#include "Shader.h"
#include "RenderTarget.h"

namespace BJEngine {

	

	class Shadow
	{
		

	public:

		
		virtual void Draw() = 0;

		virtual void GenerateView(const BJEStruct::LightDesc ld) = 0;

		virtual void BindSRV(int deltIndex) = 0;

		virtual void Close() = 0;

	protected:

		virtual void Init() = 0;

		bool isInited;

		D3D11_VIEWPORT vp;
		DepthStencil* depthStencil;
		
	};

	class OmnidirectionalShadow
		: public Shadow
	{
		
		static ID3D11Buffer* shadowCBuffer;

		BJEStruct::CubeGenerateConstantBuffer matrices;

		void Init() override;

	public:

		OmnidirectionalShadow();

		
		void Draw() override;

		void GenerateView(const BJEStruct::LightDesc ld) override;

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

		void GenerateView(const BJEStruct::LightDesc ld) override;

		void BindSRV(int deltIndex) override;

		void Close() override;

	};

}