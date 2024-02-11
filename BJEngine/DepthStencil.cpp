#include "DepthStencil.h"


namespace BJEngine
{

	bool DepthStencil::isInitedM = false;
	ID3D11DepthStencilState* DepthStencil::MaskDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::WriteDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::NoneDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::LessEqualDepthStencilState = nullptr;

	void DepthStencil::Close()
	{
		RELEASE(DepthStencilView);
	
		RELEASE(MaskDepthStencilState);
		RELEASE(NoneDepthStencilState);
		RELEASE(WriteDepthStencilState);
		RELEASE(LessEqualDepthStencilState);
	}

	bool DepthStencil::InitStencils()
	{
		if (!isInitedM)
		{
			D3D11_DEPTH_STENCIL_DESC dsDesc;
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.StencilWriteMask = 0xFF;
			dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

			GP::GetDevice()->CreateDepthStencilState(&dsDesc, &WriteDepthStencilState);

			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.StencilWriteMask = 0xFF;
			dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
			dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

			GP::GetDevice()->CreateDepthStencilState(&dsDesc, &MaskDepthStencilState);
			
			dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());

			GP::GetDevice()->CreateDepthStencilState(&dsDesc, &NoneDepthStencilState);

			
			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			GP::GetDevice()->CreateDepthStencilState(&dsDesc, &LessEqualDepthStencilState);

			isInitedM = true;
		}

		return isInitedM;
	}

	bool DepthStencil::InitView(int width, int height)
	{
		
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		ID3D11Texture2D* depthStencilBuffer;

		HRESULT hr = GP::GetDevice()->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
		if (FAILED(hr))
		{
			Log::Get()->Err("Create Simple Depth buffer error");
		}

		hr = GP::GetDevice()->CreateDepthStencilView(depthStencilBuffer, NULL, &DepthStencilView);
		if (FAILED(hr))
		{
			Log::Get()->Err("Create Simple DepthStencil buffer error");
		}

		RELEASE(depthStencilBuffer);

			
		return true;
	}

	void DepthStencil::ClearDepthStencilView()
	{
		
			GP::GetDeviceContext()->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
	}

	void DepthStencil::SetDepthStencilState(DEPTHSTENCILSTATES state)
	{
		if(isInitedM)
		{
			switch (state)
			{
			case MASK:
				GP::GetDeviceContext()->OMSetDepthStencilState(MaskDepthStencilState, 0XFF);
				break;
			
			case WRITE:
				GP::GetDeviceContext()->OMSetDepthStencilState(WriteDepthStencilState, 0XFF);
				break;
			
			case NON:
				GP::GetDeviceContext()->OMSetDepthStencilState(NoneDepthStencilState, 0XFF);
				break;
			case LESS_EQUAL:
				GP::GetDeviceContext()->OMSetDepthStencilState(LessEqualDepthStencilState, 0);
				break;
			}
		}
	}
}