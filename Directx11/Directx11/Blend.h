#pragma once
#include "pch.h"

namespace BJEngine
{


	class Blend
	{
	public:

		Blend(ID3D11Device* pd3dDevice);
		~Blend();

		static Blend* Get() { return instance; };

		void InitBlendStates(ID3D11Device* pd3dDevice);

		void DrawCullState(ID3D11DeviceContext* pImmediateContext);
		void DrawCullStateFront(ID3D11DeviceContext* pImmediateContext);
		void DrawCullStateClockFalse(ID3D11DeviceContext* pImmediateContext);

		void DrawAlphaBlend(ID3D11DeviceContext* pImmediateContext);
		void DrawNoBlend(ID3D11DeviceContext* pImmediateContext);

		void Close();

	private:

		static Blend* instance;



		D3D11_BLEND_DESC omDesc;

		bool isInitedAlphaBlending = false;
		ID3D11BlendState* alphaBlendState;

		bool isInitedNoBlending = false;
		ID3D11BlendState* noBlendState;
		


		D3D11_RASTERIZER_DESC cmdesc;

		bool isInitedShadowState;
		ID3D11RasterizerState* renStateCullFront;

		bool isInitedCullState = false;
		ID3D11RasterizerState* renStateCullNone;

		bool isInitedCullStateClockFalse = false;
		ID3D11RasterizerState* renStateCullNoneClockFalse;
		
	};


}