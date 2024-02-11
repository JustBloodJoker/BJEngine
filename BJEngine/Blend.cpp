#include "Blend.h"

namespace BJEngine
{

	Blend* Blend::instance = nullptr;

	Blend::Blend()
	{
		if (!instance)
		{
			instance = this;
			InitBlendStates();
		}
		else
			Log::Get()->Err("Blends was created");
	}

	Blend::~Blend()
	{
		
	}

	void Blend::InitBlendStates()
	{
		HRESULT hr = S_OK;

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.FrontCounterClockwise = true;
		cmdesc.CullMode = D3D11_CULL_BACK;
		hr = GP::GetDevice()->CreateRasterizerState(&cmdesc, &renStateCullBack);
		isInitedCullBackState = true;

		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			isInitedCullBackState = false;
		}

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.FrontCounterClockwise = true;
		cmdesc.CullMode = D3D11_CULL_FRONT;
		hr = GP::GetDevice()->CreateRasterizerState(&cmdesc, &renStateCullFront);
		isInitedCullFrontState = true;

		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			isInitedCullFrontState = false;
		}

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.FrontCounterClockwise = true;
		cmdesc.CullMode = D3D11_CULL_NONE;
		hr = GP::GetDevice()->CreateRasterizerState(&cmdesc, &renStateCullNone);
		isInitedCullNoneState = true;
		
		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			isInitedCullNoneState = false;
		}

		cmdesc.FillMode = D3D11_FILL_WIREFRAME;
		hr = GP::GetDevice()->CreateRasterizerState(&cmdesc, &renStateWireFrameCullNone);
		isInitedCullWireFrameState = true;
		cmdesc.FillMode = D3D11_FILL_SOLID;

		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			isInitedCullWireFrameState = false;
		}

		cmdesc.FrontCounterClockwise = false;
		hr = GP::GetDevice()->CreateRasterizerState(&cmdesc, &renStateCullNoneClockFalse);
		isInitedCullStateClockFalse = true;

		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			isInitedCullStateClockFalse = false;
		}

		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

		isInitedAlphaBlending = true;
		if (FAILED(GP::GetDevice()->CreateBlendState(&omDesc, &alphaBlendState)))
		{
			Log::Get()->Err("Blend Create Error");
			isInitedAlphaBlending = false;
		}

		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.RenderTarget[0].BlendEnable = FALSE;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		

		isInitedAlphaBlending = true;
		if (FAILED(GP::GetDevice()->CreateBlendState(&omDesc, &noBlendState)))
		{
			Log::Get()->Err("Blend Create Error");
			isInitedAlphaBlending = false;
		}

		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.AlphaToCoverageEnable = false;
		omDesc.IndependentBlendEnable = false;
		omDesc.RenderTarget[0].BlendEnable = true;
		omDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_DEST_COLOR;
		omDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		omDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		omDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		
		isInitedTransparencyBlending = true;
		if (FAILED(GP::GetDevice()->CreateBlendState(&omDesc, &transparencyBlendState)))
		{
			Log::Get()->Err("Blend Create Error");
			isInitedTransparencyBlending = false;
		}

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.CullMode = D3D11_CULL_BACK;
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.DepthClipEnable = true;

		isInitedShadowState = true;
		if (FAILED(GP::GetDevice()->CreateRasterizerState(&cmdesc, &renStateCullShadow)))
		{
			Log::Get()->Err("Blend Create Error");
			isInitedShadowState = false;
		};

	}

	void Blend::DrawCullNoneState()
	{
		if (isInitedCullNoneState)
			GP::GetDeviceContext()->RSSetState(renStateCullNone);
	}

	void Blend::DrawCullFrontState()
	{
		if (isInitedCullFrontState)
			GP::GetDeviceContext()->RSSetState(renStateCullFront);
	}

	void Blend::DrawCullBackState()
	{
		if (isInitedCullBackState)
			GP::GetDeviceContext()->RSSetState(renStateCullBack);
	}

	void Blend::DrawWireFrameCullState( )
	{
		if (isInitedCullWireFrameState)
			GP::GetDeviceContext()->RSSetState(renStateWireFrameCullNone);
	}

	void Blend::DrawCullStateFront( )
	{
		if (isInitedShadowState)
			GP::GetDeviceContext()->RSSetState(renStateCullShadow);
	}

	void Blend::DrawCullStateClockFalse( )
	{
		if (isInitedCullStateClockFalse)
			GP::GetDeviceContext()->RSSetState(renStateCullNoneClockFalse);
	}

	void Blend::DrawAlphaBlend( )
	{
		if (isInitedAlphaBlending)
			GP::GetDeviceContext()->OMSetBlendState(alphaBlendState, 0, 0xffffffff);
	}

	void Blend::DrawNoBlend( )
	{
		if (isInitedAlphaBlending)
			GP::GetDeviceContext()->OMSetBlendState(noBlendState, 0, 0xffffffff);
	}

	void Blend::DrawTransparencyBlend( )
	{
		if (isInitedTransparencyBlending)
			GP::GetDeviceContext()->OMSetBlendState(transparencyBlendState, 0, 0xffffffff);
	}

	void Blend::Close()
	{
		RELEASE(alphaBlendState);
		RELEASE(noBlendState);
		RELEASE(renStateCullNone);
		RELEASE(renStateCullNoneClockFalse);
	}


}