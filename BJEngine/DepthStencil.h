#pragma once
#include "pch.h"


namespace BJEngine
{
	enum DEPTHSTENCILSTATES
	{
		MASK,
		WRITE,
		NON,
		LESS_EQUAL,

	};

	class DepthStencil
	{
	public:

		DepthStencil() = default;
		~DepthStencil() = default;

		void Close();

		static bool InitStencils();
		bool InitView(int width, int height);

		void ClearDepthStencilView();
		ID3D11DepthStencilView* GetDepthStencil() { return DepthStencilView; }
		
		static void SetDepthStencilState(DEPTHSTENCILSTATES state);
		
	private:

		static bool isInitedM;
		
		ID3D11DepthStencilView* DepthStencilView;

		static ID3D11DepthStencilState* MaskDepthStencilState;
		static ID3D11DepthStencilState* NoneDepthStencilState;
		static ID3D11DepthStencilState* WriteDepthStencilState;
		static ID3D11DepthStencilState* LessEqualDepthStencilState;

	};


}
