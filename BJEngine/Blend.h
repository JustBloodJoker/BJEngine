#pragma once
#include "pch.h"

namespace BJEngine
{


	class Blend
	{
	public:

		Blend();
		~Blend();

		static Blend* Get() { return instance; };

		void InitBlendStates();

		void DrawCullNoneState();
		void DrawCullFrontState();
		void DrawCullBackState();
		void DrawWireFrameCullState();
		void DrawCullStateFront();
		void DrawCullStateClockFalse();

		void DrawAlphaBlend();
		void DrawNoBlend();
		void DrawTransparencyBlend();


		void Close();

	private:

		static Blend* instance;



		D3D11_BLEND_DESC omDesc;

		bool isInitedAlphaBlending = false;
		ID3D11BlendState* alphaBlendState;

		bool isInitedNoBlending = false;
		ID3D11BlendState* noBlendState;

		bool isInitedTransparencyBlending = false;
		ID3D11BlendState* transparencyBlendState;


		D3D11_RASTERIZER_DESC cmdesc;

		bool isInitedShadowState;
		ID3D11RasterizerState* renStateCullShadow;

		bool isInitedCullNoneState = false;
		ID3D11RasterizerState* renStateCullNone;

		bool isInitedCullFrontState = false;
		ID3D11RasterizerState* renStateCullFront;
		
		bool isInitedCullBackState = false;
		ID3D11RasterizerState* renStateCullBack;

		bool isInitedCullWireFrameState = false;
		ID3D11RasterizerState* renStateWireFrameCullNone;

		bool isInitedCullStateClockFalse = false;
		ID3D11RasterizerState* renStateCullNoneClockFalse;

	};


}