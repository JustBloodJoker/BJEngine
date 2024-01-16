#include "Element.h"
#include "Helper.h"
#include "Blend.h"
#include "Object.h"

namespace BJEngine
{
	int Element::count = 0;
	Shader* Element::glowShader = nullptr;

	Element::Element()
	{


	}

	Element::Element(std::vector<BJEStruct::ModelVertex> v, std::vector<WORD> i, Materials* material, dx::XMVECTOR min, dx::XMVECTOR max, ID3D11Device* pd3dDevice)
	{
		vertices = std::move(v);
		indices = std::move(i);
		this->pMaterial = material; material = nullptr;
		minLocal = min;
		maxLocal = max;
		name = std::string("Element ") + std::to_string(Element::count);
		count++;

		if (!Element::glowShader)
		{
			Element::glowShader = new BJEngine::Shader(L"shaders\\GlowPS.hlsl", L"shaders\\GlowVS.hlsl", "VS", "PS");
			static D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};
			Element::glowShader->SetInputLayout(layout, ARRAYSIZE(layout));
			Element::glowShader->Init(pd3dDevice);
		}
	}

	Element::~Element()
	{


	}

	void Element::Close()
	{
		vertices.clear();
		indices.clear();
		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);
		name.clear();
	}

	void Element::Init(ID3D11Device* pd3dDevice, ID3D11Buffer* pConstantBuffer)
	{
		pVertexBuffer = Helper::InitVertexBuffer(pd3dDevice, sizeof(BJEStruct::ModelVertex) * vertices.size(), &vertices[0]);
		pIndexBuffer =  Helper::InitIndicesBuffer(pd3dDevice, sizeof(WORD) * indices.size(), &indices[0]);
		this->pConstantBuffer = pConstantBuffer;
		pGlowConstantBuffer = Helper::InitConstantBuffer<CBuf>(pd3dDevice);
	}

	void Element::Draw(ID3D11DeviceContext* pImmediateContext, UINT* stride, UINT* offset, dx::XMMATRIX mainWorldMatrix, dx::XMMATRIX outLine, dx::XMMATRIX viewMatrix, dx::XMMATRIX projectionMatrix, dx::XMMATRIX* lView, dx::XMMATRIX* lProjection)
	{	
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, stride, offset);
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

		pMaterial->Draw(pImmediateContext, 2);

		Object::ConstantBuffer cb;
		cb.WVP = dx::XMMatrixTranspose(mainWorldMatrix * viewMatrix * projectionMatrix);
		cb.World = XMMatrixTranspose(mainWorldMatrix);
		cb.ViewMatrix = viewMatrix;
		cb.projectionMatrix = projectionMatrix;

		for (int j = 0; j < MAX_LIGHT_NUM; j++)
		{
			cb.lView[j] = dx::XMMatrixTranspose(lView[j]);
			cb.lProj[j] = dx::XMMatrixTranspose(lProjection[j]);
		}

		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
		pImmediateContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

		if (focusedState)
		{
			Blend::Get()->DrawCullNoneState(pImmediateContext);

			DepthStencil::GetWritedDepthStencilState();
			pImmediateContext->DrawIndexed(indices.size(), 0, 0);
			
			pImmediateContext->IASetInputLayout(Element::glowShader->GetInputLayout());
			pImmediateContext->VSSetShader(Element::glowShader->GetVertexShader(), NULL, 0);
			pImmediateContext->PSSetShader(Element::glowShader->GetPixelShader(), NULL, 0);

			CBuf dd;
			dd.WVP =  dx::XMMatrixTranspose(outLine * viewMatrix * projectionMatrix);

			pImmediateContext->UpdateSubresource(pGlowConstantBuffer, 0, NULL, &dd, 0, 0);
			pImmediateContext->VSSetConstantBuffers(0, 1, &pGlowConstantBuffer);

			DepthStencil::GetMaskedDepthStencilState();
			pImmediateContext->DrawIndexed(indices.size(), 0, 0);	
		}
		else
		{
			Blend::Get()->DrawCullFrontState(pImmediateContext);
			DepthStencil::ResetDepthStencilState();
			pImmediateContext->DrawIndexed(indices.size(), 0, 0);

		}
	}

	void Element::MinDraw(ID3D11DeviceContext* pImmediateContext, UINT* stride, UINT* offset)
	{
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, stride, offset);
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pImmediateContext->DrawIndexed(indices.size(), 0, 0);
	}

	dx::XMVECTOR Element::GetMinLocal()
	{
		return minLocal;
	}

	dx::XMVECTOR Element::GetMaxLocal()
	{
		return maxLocal;
	}

	std::string Element::GetName() const
	{
		return name;
	}

	void Element::SetFocusState(bool state)
	{
		focusedState = state;
	}


}