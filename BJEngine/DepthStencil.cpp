#include "DepthStencil.h"


namespace BJEngine
{

	bool DepthStencil::isInitedM = false;
	ID3D11DepthStencilState* DepthStencil::MaskDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::WriteDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::NoneDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::LessEqualDepthStencilState = nullptr;
	ID3D11DepthStencilState* DepthStencil::ReadOnlyDepthStencilState = nullptr;

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
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;

			GP::GetDevice()->CreateDepthStencilState(&dsDesc, &ReadOnlyDepthStencilState);


			ZeroMemory(&dsDesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
			dsDesc.DepthEnable = true;
			dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			dsDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

			GP::GetDevice()->CreateDepthStencilState(&dsDesc, &LessEqualDepthStencilState);

			isInitedM = true;
		}

		return isInitedM;
	}

	bool DepthStencil::InitView(int width, int height, bool MSAA,
		UINT arraySize, DXGI_FORMAT format, UINT MiscFlags, UINT BindFlags,
		bool isShaderResourseViewBind, bool isShadow)
	{
		
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = width;
		depthStencilDesc.Height = height;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = arraySize;
		if (isShadow)
		{
			if(format == DXGI_FORMAT_D32_FLOAT)
				depthStencilDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		}
		else
		{
			depthStencilDesc.Format = format;
		}

		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = BindFlags;

		isShaderResourseViewBind ? depthStencilDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | BindFlags : true ;
		
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = MiscFlags;



		ID3D11Texture2D* depthStencilBuffer;

		HRESULT hr = GP::GetDevice()->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
		if (FAILED(hr))
		{
			Log::Get()->Err("Create Simple Depth buffer error");
		}
		
		if (isShaderResourseViewBind)
		{
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
			if (isShadow)
			{
				if (format == DXGI_FORMAT_D32_FLOAT)
					srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
			}
			else
			{
				srvDesc.Format = format;
			}
			if (MiscFlags == D3D11_RESOURCE_MISC_TEXTURECUBE)
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
				srvDesc.TextureCube.MipLevels = 1;
			}
			else
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
				srvDesc.Texture2D.MipLevels = 1;
				srvDesc.Texture2D.MostDetailedMip = 0;
			}
			hr = GP::GetDevice()->CreateShaderResourceView(depthStencilBuffer, &srvDesc, &srv);
		}

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = CD3D11_DEPTH_STENCIL_VIEW_DESC();
		if (isShadow)
		{
			descView.Format = format;
			descView.Flags = 0;
			if (MiscFlags == D3D11_RESOURCE_MISC_TEXTURECUBE)
				descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
			else
				descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;

			descView.Texture2DArray.MipSlice = 0;
			descView.Texture2DArray.ArraySize = arraySize;
			descView.Texture2DArray.FirstArraySlice = 0;
			hr = GP::GetDevice()->CreateDepthStencilView(depthStencilBuffer, &descView, &DepthStencilView);
		}
		else
		{
			
			descView.Format = format;
			if (MSAA)
			{
				descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DMS;
			}
			else
			{
				descView.Texture2D.MipSlice = 0;
				descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
			}
			hr = GP::GetDevice()->CreateDepthStencilView(depthStencilBuffer, &descView, &DepthStencilView);
		}
		
		if (FAILED(hr))
		{
			Log::Get()->Err("Create Simple DepthStencil buffer error");
		}

		


		RELEASE(depthStencilBuffer);

			
		return true;
	}

	ID3D11ShaderResourceView*& DepthStencil::GetSRV()
	{
		return srv;
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
			case READ:
				GP::GetDeviceContext()->OMSetDepthStencilState(ReadOnlyDepthStencilState, 0);
				break;

			}
		}
	}
}