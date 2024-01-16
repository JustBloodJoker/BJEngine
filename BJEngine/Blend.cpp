#include "Blend.h"

namespace BJEngine
{

	Blend* Blend::instance = nullptr;

	Blend::Blend(ID3D11Device* pd3dDevice)
	{
		if (!instance)
		{
			instance = this;
			InitBlendStates(pd3dDevice);
		}
		else
			Log::Get()->Err("Blends was created");
	}

	Blend::~Blend()
	{
		
	}

	void Blend::InitBlendStates(ID3D11Device* pd3dDevice)
	{
		HRESULT hr = S_OK;

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.FrontCounterClockwise = true;
		cmdesc.CullMode = D3D11_CULL_BACK;
		hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullBack);
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
		hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullFront);
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
		hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullNone);
		isInitedCullNoneState = true;
		
		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			isInitedCullNoneState = false;
		}

		cmdesc.FillMode = D3D11_FILL_WIREFRAME;
		hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateWireFrameCullNone);
		isInitedCullWireFrameState = true;
		cmdesc.FillMode = D3D11_FILL_SOLID;

		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			isInitedCullWireFrameState = false;
		}

		cmdesc.FrontCounterClockwise = false;
		hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullNoneClockFalse);
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
		if (FAILED(pd3dDevice->CreateBlendState(&omDesc, &alphaBlendState)))
		{
			Log::Get()->Err("Blend Create Error");
			isInitedAlphaBlending = false;
		}

		ZeroMemory(&omDesc, sizeof(D3D11_BLEND_DESC));
		omDesc.RenderTarget[0].BlendEnable = FALSE;
		omDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		

		isInitedAlphaBlending = true;
		if (FAILED(pd3dDevice->CreateBlendState(&omDesc, &noBlendState)))
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
		if (FAILED(pd3dDevice->CreateBlendState(&omDesc, &transparencyBlendState)))
		{
			Log::Get()->Err("Blend Create Error");
			isInitedTransparencyBlending = false;
		}

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.CullMode = D3D11_CULL_BACK;
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.DepthClipEnable = true;

		isInitedShadowState = true;
		if (FAILED(pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullShadow)))
		{
			Log::Get()->Err("Blend Create Error");
			isInitedShadowState = false;
		};

	}

	void Blend::DrawCullNoneState(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedCullNoneState)
			pImmediateContext->RSSetState(renStateCullNone);
	}

	void Blend::DrawCullFrontState(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedCullFrontState)
			pImmediateContext->RSSetState(renStateCullFront);
	}

	void Blend::DrawCullBackState(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedCullBackState)
			pImmediateContext->RSSetState(renStateCullBack);
	}

	void Blend::DrawWireFrameCullState(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedCullWireFrameState)
			pImmediateContext->RSSetState(renStateWireFrameCullNone);
	}

	void Blend::DrawCullStateFront(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedShadowState)
			pImmediateContext->RSSetState(renStateCullShadow);
	}

	void Blend::DrawCullStateClockFalse(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedCullStateClockFalse)
			pImmediateContext->RSSetState(renStateCullNoneClockFalse);
	}

	void Blend::DrawAlphaBlend(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedAlphaBlending)
			pImmediateContext->OMSetBlendState(alphaBlendState, 0, 0xffffffff);
	}

	void Blend::DrawNoBlend(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedAlphaBlending)
			pImmediateContext->OMSetBlendState(noBlendState, 0, 0xffffffff);
	}

	void Blend::DrawTransparencyBlend(ID3D11DeviceContext* pImmediateContext)
	{
		if (isInitedTransparencyBlending)
			pImmediateContext->OMSetBlendState(transparencyBlendState, 0, 0xffffffff);
	}

	void Blend::Close()
	{
		RELEASE(alphaBlendState);
		RELEASE(noBlendState);
		RELEASE(renStateCullNone);
		RELEASE(renStateCullNoneClockFalse);
	}


}