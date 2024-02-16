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
		bool InitView(int width, int height,bool MSAA = false, UINT arraySize = 1, DXGI_FORMAT format = DXGI_FORMAT_D24_UNORM_S8_UINT, UINT MiscFlags = 0, UINT BindFlags = D3D11_BIND_DEPTH_STENCIL,
			bool isShaderResourseViewBind = false, bool isShadow = false);
		
		ID3D11ShaderResourceView*& GetSRV();
		void ClearDepthStencilView();
		ID3D11DepthStencilView* GetDepthStencil() { return DepthStencilView; }
		
		static void SetDepthStencilState(DEPTHSTENCILSTATES state);
		
	private:

		static bool isInitedM;
		
		ID3D11DepthStencilView* DepthStencilView;
		ID3D11ShaderResourceView* srv = nullptr;

		static ID3D11DepthStencilState* MaskDepthStencilState;
		static ID3D11DepthStencilState* NoneDepthStencilState;
		static ID3D11DepthStencilState* WriteDepthStencilState;
		static ID3D11DepthStencilState* LessEqualDepthStencilState;

		
	};


}
