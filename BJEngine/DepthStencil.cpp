#include "DepthStencil.h"


namespace BJEngine
{

	ID3D11RenderTargetView* DepthStencil::mainRTV = nullptr;
	ID3D11Device* DepthStencil::pd3dDevice = nullptr;
	ID3D11DeviceContext* DepthStencil::pImmediateContext = nullptr;

	bool DepthStencil::isInitedS = false, DepthStencil::isInitedM = false;
	ID3D11DepthStencilView* DepthStencil::DepthStencilView = nullptr;
	ID3D11DepthStencilState* DepthStencil::MaskDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::WriteDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::NoneDepthStencilState = nullptr;

	void DepthStencil::Close()
	{
		RELEASE(DepthStencilView);
		RELEASE(MaskDepthStencilState);
		RELEASE(NoneDepthStencilState);
		RELEASE(WriteDepthStencilState);
	}

	bool DepthStencil::InitStencils(ID3D11RenderTargetView* rtv, ID3D11Device* dev, ID3D11DeviceContext* con)
	{
		DepthStencil::mainRTV = rtv;
		DepthStencil::pd3dDevice = dev;
		DepthStencil::pImmediateContext = con;


		if (!isInitedS)
		{
			D3D11_TEXTURE2D_DESC depthStencilDesc;
			depthStencilDesc.Width = BJEUtils::GetWindowWidth();
			depthStencilDesc.Height = BJEUtils::GetWindowHeight();
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

			HRESULT hr = pd3dDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
			if (FAILED(hr))
			{
				Log::Get()->Err("Create Simple Depth buffer error");
			}

			hr = pd3dDevice->CreateDepthStencilView(depthStencilBuffer, NULL, &DepthStencilView);
			if (FAILED(hr))
			{
				Log::Get()->Err("Create Simple DepthStencil buffer error");
			}

			RELEASE(depthStencilBuffer);

			isInitedS = true;

		}

		if (!isInitedM)
		{
			// Create depth stencil state corresponding to step one of the article.
			D3D11_DEPTH_STENCIL_DESC dsDesc;
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

			// Stencil test parameters
			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.StencilWriteMask = 0xFF;

			// Stencil operations if pixel is front-facing.

			// Keep original value on fail.
			dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			// Write to the stencil on pass.
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_INCR_SAT;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

			// Stencil operations if pixel is back-facing.
			// Since we do not care about back-facing pixels, always keep original value.
			dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

			// Create the depth stencil state.
			pd3dDevice->CreateDepthStencilState(&dsDesc, &WriteDepthStencilState);

			// Create depth stencil state corresponding to step two of the article.
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS;

			dsDesc.StencilEnable = true;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.StencilWriteMask = 0xFF;

			// It does not matter what we write since we are not using the values after this step.
			// In other words, we are only using the values to mask pixels.
			dsDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			// The stencil test passes if the passed parameter is equal to value in the buffer.
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;

			// Again, we do not care about back-facing pixels.
			dsDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
			dsDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;

			// Create the depth stencil state.
			pd3dDevice->CreateDepthStencilState(&dsDesc, &MaskDepthStencilState);


			/*D3D11_DEPTH_STENCIL_DESC dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
			dsDesc.StencilEnable = TRUE;
			dsDesc.StencilWriteMask = 0xFF;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_REPLACE;
			

			pd3dDevice->CreateDepthStencilState(&dsDesc, &WriteDepthStencilState);
			
			dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());
			dsDesc.DepthEnable = FALSE;
			dsDesc.StencilEnable = TRUE;
			dsDesc.StencilReadMask = 0xFF;
			dsDesc.FrontFace.StencilFunc = D3D11_COMPARISON_NOT_EQUAL;
			dsDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;

			pd3dDevice->CreateDepthStencilState(&dsDesc, &MaskDepthStencilState);*/
			
			dsDesc = CD3D11_DEPTH_STENCIL_DESC(CD3D11_DEFAULT());

			pd3dDevice->CreateDepthStencilState(&dsDesc, &NoneDepthStencilState);

			isInitedM = true;
		}

		return isInitedS && isInitedM;
	}

	ID3D11DepthStencilView* DepthStencil::ClearDepthStencil()
	{
		if (isInitedS)
		{
			pImmediateContext->ClearDepthStencilView(DepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			return DepthStencilView;
		}
		return nullptr;
	}

	void DepthStencil::GetMaskedDepthStencilState()
	{
		if (isInitedM)
		{
			pImmediateContext->OMSetDepthStencilState(MaskDepthStencilState, 0XFF);
			pImmediateContext->OMSetRenderTargets(1, &mainRTV, DepthStencilView);
		}
	}

	void DepthStencil::GetWritedDepthStencilState()
	{
		if (isInitedM)
		{
			pImmediateContext->OMSetDepthStencilState(WriteDepthStencilState, 0XFF);
			pImmediateContext->OMSetRenderTargets(1, &mainRTV, DepthStencilView);
		}
	}

	void DepthStencil::ResetDepthStencilState()
	{
		if (isInitedM)
		{
			pImmediateContext->OMSetDepthStencilState(NoneDepthStencilState, 0XFF);
			pImmediateContext->OMSetRenderTargets(1, &mainRTV, DepthStencilView);
		}
	}


}