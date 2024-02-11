#include "Element.h"
#include "Helper.h"
#include "Blend.h"
#include "Object.h"
#include "UI.h"

namespace BJEngine
{
	int Element::count = 0;
	
	Element::Element()
	{


	}

	Element::Element(std::vector<BJEStruct::ModelVertex> v, std::vector<WORD> i, Materials* material,
		dx::XMVECTOR min, dx::XMVECTOR max)
	{
		world = dx::XMMatrixIdentity();

		vertices = std::move(v);
		indices = std::move(i);
		this->pMaterial = material; material = nullptr;
		minLocal = min;
		maxLocal = max;
		name = std::string("Element ") + std::to_string(Element::count);
		count++;

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

	void Element::Init(std::vector<ID3D11Buffer*>* ConstantBuffers)
	{
		pVertexBuffer = Helper::InitVertexBuffer(GP::GetDevice(), sizeof(BJEStruct::ModelVertex) * vertices.size(), &vertices[0]);
		pIndexBuffer =  Helper::InitIndicesBuffer(GP::GetDevice(), sizeof(WORD) * indices.size(), &indices[0]);
		this->ConstantBuffers = ConstantBuffers;
		
		drawing = false;
	}

	void Element::Draw(CameraDesc cam, dx::XMMATRIX* lView, dx::XMMATRIX* lProjection)
	{	
		objectBox.CreateFromPoints(objectBox, dx::XMVector3Transform(maxLocal, world),
			dx::XMVector3Transform(minLocal, world)
		);
		
		if (!drawing && ((cam.frustum.Intersects(objectBox) || cam.frustum.Contains(objectBox))))
		{
			UINT stride = sizeof(BJEStruct::ModelVertex);
			UINT offset = 0;
			drawing = true;

			GP::BindShader(GP::MODEL_SHADER);
			GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			GP::GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
			GP::GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			pMaterial->Draw(2);

			BJEStruct::VertexConstantBuffer cb;
			cb.WVP = dx::XMMatrixTranspose(world * cam.viewMatrix * cam.projectionMatrix);
			cb.World = XMMatrixTranspose(world);
			cb.ViewMatrix = cam.viewMatrix;
			cb.projectionMatrix = cam.projectionMatrix;

			for (int j = 0; j < MAX_LIGHT_NUM; j++)
			{
				cb.lView[j] = dx::XMMatrixTranspose(lView[j]);
				cb.lProj[j] = dx::XMMatrixTranspose(lProjection[j]);
			}
			
			GP::GetDeviceContext()->VSSetConstantBuffers(1, 1, &(*ConstantBuffers)[0]);
			GP::GetDeviceContext()->UpdateSubresource((*ConstantBuffers)[0], 0, NULL, &cb, 0, 0);
			


			if (focusedState)
			{
				Blend::Get()->DrawCullNoneState();

				DepthStencil::SetDepthStencilState(WRITE);
				GP::GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);

				GP::BindShader(GP::GLOW_MODEL_SHADER);

				BJEStruct::WVPConstantBuffer dd;
				dd.WVP = dx::XMMatrixTranspose((dx::XMMatrixScaling(1.05f, 1.05f, 1.05f) + world) * cam.viewMatrix * cam.projectionMatrix);

				GP::GetDeviceContext()->UpdateSubresource((*ConstantBuffers)[1], 0, NULL, &dd, 0, 0);
				GP::GetDeviceContext()->VSSetConstantBuffers(0, 1, &(*ConstantBuffers)[1]);

				DepthStencil::SetDepthStencilState(MASK);

			}
			else
			{
				Blend::Get()->DrawCullNoneState();			
				DepthStencil::SetDepthStencilState(NON);
			}


			GP::GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
			
			drawing = false;

		}
	}

	void Element::MinDraw(dx::BoundingFrustum frustum)
	{
		if (!drawing && ((frustum.Intersects(objectBox) || frustum.Contains(objectBox))))
		{
			UINT stride = sizeof(BJEStruct::ModelVertex);
			UINT offset = 0;
			drawing = true;
			GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			GP::GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
			GP::GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
			GP::GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
			drawing = false;
		}
	}

	std::string Element::GetName() const
	{
		return name;
	}

	void Element::SetFocusState(bool state)
	{
		focusedState = state;
	}

	dx::XMFLOAT3 Element::GetWorldPosition() const
	{
		dx::XMVECTOR scale , rotate, transpose;
		dx::XMMatrixDecompose(&scale, &rotate, &transpose, world);

		transpose *= scale;

		return dx::XMFLOAT3(transpose.vector4_f32[0], transpose.vector4_f32[1], transpose.vector4_f32[2]);
	}


}