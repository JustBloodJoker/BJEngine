#pragma once
#include "pch.h"


namespace BJEngine
{


	class DepthStencil
	{
	public:

		DepthStencil() = default;
		~DepthStencil() = default;

		void Close();

		static bool InitStencils(ID3D11RenderTargetView*, ID3D11Device*, ID3D11DeviceContext*);

		static ID3D11DepthStencilView* ClearDepthStencil();
		static void GetMaskedDepthStencilState();
		static void GetWritedDepthStencilState();
		static void ResetDepthStencilState();
		static void ResetStencilState()
		{
			pImmediateContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_STENCIL, 1.0f, 0);
		}
		
	private:


		static ID3D11RenderTargetView* mainRTV;
		static ID3D11Device* pd3dDevice;
		static ID3D11DeviceContext* pImmediateContext;

		static bool isInitedS, isInitedM;
		static ID3D11DepthStencilView* DepthStencilView;
		static ID3D11DepthStencilState* MaskDepthStencilState;
		static ID3D11DepthStencilState* NoneDepthStencilState;
		static ID3D11DepthStencilState* WriteDepthStencilState;

	};


}
