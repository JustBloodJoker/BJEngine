#include "Element.h"
#include "Helper.h"
#include "Blend.h"
#include "Object.h"
#include "UI.h"

namespace BJEngine
{
	ID3D11Buffer* Element::pConstantBuffer = nullptr;
	ID3D11Buffer* Element::pWorldConstantBuffer = nullptr;
	int Element::count = 0;
	
	void Element::BindConstantBuffer()
	{
		if (pConstantBuffer == nullptr)
			pConstantBuffer = Helper::InitConstantBuffer<BJEStruct::VertexConstantBuffer>(GP::GetDevice());

		GP::GetDeviceContext()->VSSetConstantBuffers(1, 1, &pConstantBuffer);
	}

	Element::Element()
	{
		
	}

	Element::Element(std::vector<BJEStruct::ModelVertex> v, std::vector<WORD> i, Materials* material,
		dx::XMVECTOR min, dx::XMVECTOR max)
	{
		if(pConstantBuffer == nullptr)
			pConstantBuffer = Helper::InitConstantBuffer<BJEStruct::VertexConstantBuffer>(GP::GetDevice());

		pWorldConstantBuffer = Helper::InitConstantBuffer<BJEStruct::WVPConstantBuffer>(GP::GetDevice());

		world = dx::XMMatrixIdentity();

		vertices = std::move(v);
		indices = std::move(i);
		this->pMaterial = material; material = nullptr;
		minLocal = min;
		maxLocal = max;
		name = std::string("Element ") + std::to_string(Element::count);
		count++;
		Init();

		priority = this->pMaterial->GenRenderPriority();
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

	void Element::Init()
	{
		pVertexBuffer = Helper::InitVertexBuffer(GP::GetDevice(), sizeof(BJEStruct::ModelVertex) * vertices.size(), &vertices[0]);
		pIndexBuffer =  Helper::InitIndicesBuffer(GP::GetDevice(), sizeof(WORD) * indices.size(), &indices[0]);
		
		drawing = false;
	}

	void Element::Draw(CameraDesc cam)
	{	
		objectBox.CreateFromPoints(objectBox, dx::XMVector3Transform(maxLocal, world),
			dx::XMVector3Transform(minLocal, world)
		);
		
		Binds();

		if (!drawing && ((cam.frustum.Intersects(objectBox) || cam.frustum.Contains(objectBox))) || !frustumCheck)
		{
			UINT stride = sizeof(BJEStruct::ModelVertex);
			UINT offset = 0;
			drawing = true;

			GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			GP::GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
			GP::GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			pMaterial->Draw();

			BJEStruct::VertexConstantBuffer cb;
			cb.WVP = dx::XMMatrixTranspose(world * cam.viewMatrix * cam.projectionMatrix);
			cb.World = XMMatrixTranspose(world);
			cb.ViewMatrix = cam.viewMatrix;
		
			
			GP::GetDeviceContext()->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
			
			Blend::Get()->DrawCullNoneState();			
			DepthStencil::SetDepthStencilState(NON);
		
			GP::GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);
			
			drawing = false;

		}
	}

	void Element::DrawShadow()
	{
		if (!drawing && priority != 2)
		{
			UINT stride = sizeof(BJEStruct::ModelVertex);
			UINT offset = 0;
			GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			GP::GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
			GP::GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			
			drawing = true;
			BJEStruct::WVPConstantBuffer cb;
			cb.WVP = XMMatrixTranspose(world);
			GP::GetDeviceContext()->VSSetConstantBuffers(1, 1, &pWorldConstantBuffer);
			GP::GetDeviceContext()->UpdateSubresource(pWorldConstantBuffer, 0, NULL, &cb, 0, 0);

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

	const int Element::GetPriorityRender() const
	{
		return priority;
	}

	void Element::Binds()
	{
		
	}


}