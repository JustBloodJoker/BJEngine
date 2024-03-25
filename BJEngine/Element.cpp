#include "Element.h"
#include "Helper.h"
#include "Blend.h"
#include "Object.h"
#include "UI.h"

namespace BJEngine
{
	ID3D11Buffer* BaseElement::pConstantBuffer = nullptr;
	int Element::count = 0;
	
	void BaseElement::BindConstantBuffer()
	{
		if (pConstantBuffer == nullptr)
			pConstantBuffer = Helper::InitConstantBuffer<BJEStruct::VertexConstantBuffer>(GP::GetDevice());

		GP::GetDeviceContext()->VSSetConstantBuffers(1, 1, &pConstantBuffer);
		GP::GetDeviceContext()->GSSetConstantBuffers(1, 1, &pConstantBuffer);
	}

	ID3D11Buffer*& BaseElement::GetConstantBuffer()
	{
		return pConstantBuffer;
	}

	Element::Element()
	{
		
	}

	Element::Element(std::vector<BJEStruct::ModelVertex>&& v, std::vector<WORD>&& i, Materials* material,
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
		Init();

		priority = static_cast<ELEMENT_PRIORITY>(this->pMaterial->GenRenderPriority());
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

	void Element::Draw(CameraDesc cam, ID3D11DeviceContext* context)
	{	
		objectBox.CreateFromPoints(objectBox, dx::XMVector3Transform(maxLocal, world),
			dx::XMVector3Transform(minLocal, world)
		);

		if (!drawing && ((cam.frustum.Intersects(objectBox) || cam.frustum.Contains(objectBox))) && frustumCheck)
		{
			UINT stride = sizeof(BJEStruct::ModelVertex);
			UINT offset = 0;
			drawing = true;

			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);

			pMaterial->Draw();

			BJEStruct::VertexConstantBuffer cb;
			cb.WVP = dx::XMMatrixTranspose(world * cam.viewMatrix * cam.projectionMatrix);
			cb.World = XMMatrixTranspose(world);
		
			
			context->UpdateSubresource(BaseElement::GetConstantBuffer(), 0, NULL, &cb, 0, 0);
			
			Blend::Get()->DrawCullNoneState();			
			DepthStencil::SetDepthStencilState(NON);
		
			context->DrawIndexed(indices.size(), 0, 0);
			
			drawing = false;

		}
	}

	void Element::DrawShadow(ID3D11DeviceContext* context)
	{
		if (!drawing && priority == 1)
		{
			UINT stride = sizeof(BJEStruct::ModelVertex);
			UINT offset = 0;
			context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
			context->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
			context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
			drawing = true;
			BJEStruct::WVPConstantBuffer cb;
			cb.WVP = XMMatrixTranspose(world);
			context->UpdateSubresource(BaseElement::GetConstantBuffer(), 0, NULL, &cb, 0, 0);

			context->DrawIndexed(indices.size(), 0, 0);
			drawing = false;
		}
	}

	std::string Element::GetMaterialName()
	{
		return pMaterial->GetName();
	}

	std::vector<BJEStruct::ModelVertex> Element::GetVertices() const
	{
		return vertices;
	}

	std::vector<WORD> Element::GetIndices() const
	{
		return indices;
	}


	ElementSkyBox::ElementSkyBox(Textures* texture)
	{
		vertices =
		{
			BJEStruct::VertexPosOnly(-1.0f, 1.0f, -1.0f),
			BJEStruct::VertexPosOnly(1.0f, 1.0f, -1.0f),
			BJEStruct::VertexPosOnly(1.0f, 1.0f, 1.0f),
			BJEStruct::VertexPosOnly(-1.0f, 1.0f, 1.0f),
			BJEStruct::VertexPosOnly(-1.0f, -1.0f, -1.0f),
			BJEStruct::VertexPosOnly(1.0f, -1.0f, -1.0f),
			BJEStruct::VertexPosOnly(1.0f, -1.0f, 1.0f),
			BJEStruct::VertexPosOnly(-1.0f, -1.0f, 1.0f),
		};

		indices =
		{
			3,1,0,
			2,1,3,

			0,5,4,
			1,5,0,

			3,4,7,
			0,4,3,

			1,6,5,
			2,6,1,

			2,7,6,
			3,7,2,

			6,4,5,
			7,4,6,
		};

		ttext = texture;
		ttext->InitCubeMap();

		name = std::string("Sky box");
		priority = SKYBOX;

		Init();
	}

	ElementSkyBox::~ElementSkyBox()
	{

	}

	void ElementSkyBox::Close()
	{
		indices.clear();
		vertices.clear();
		CLOSE(ttext);

		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);
	}

	void ElementSkyBox::Init()
	{
		pVertexBuffer = Helper::InitVertexBuffer(GP::GetDevice(), sizeof(BJEStruct::VertexPosOnly) * vertices.size(), &vertices[0]);
		pIndexBuffer = Helper::InitIndicesBuffer(GP::GetDevice(), sizeof(WORD) * indices.size(), &indices[0]);
		
		

		drawing = false;
	}

	void ElementSkyBox::Draw(CameraDesc cam, ID3D11DeviceContext* context)
	{
		GP::BindShader(GP::BACKGROUND_SHADER);

		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		world = dx::XMMatrixIdentity();

		world = dx::XMMatrixScaling(10000.0f, 10000.0f, 10000.0f) *

			dx::XMMatrixTranslation(dx::XMVectorGetX(cam.eye), dx::XMVectorGetY(cam.eye), dx::XMVectorGetZ(cam.eye));

		UINT stride = sizeof(BJEStruct::VertexPosOnly);
		UINT offset = 0;

		context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		context->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		BJEStruct::VertexConstantBuffer cb;
		cb.WVP = XMMatrixTranspose(world * cam.viewMatrix * cam.projectionMatrix);
		cb.World = XMMatrixTranspose(world);

		context->UpdateSubresource(BaseElement::GetConstantBuffer(), 0, NULL, &cb, 0, 0);

		context->PSSetShaderResources(0, 1, &ttext->GetTexture());
		
		context->PSSetSamplers(0, 1, Textures::GetWrapState());

		DepthStencil::SetDepthStencilState(LESS_EQUAL);

		Blend::Get()->DrawNoBlend();
		Blend::Get()->DrawCullStateClockFalse();

		context->DrawIndexed(indices.size(), 0, 0);
		context->OMSetDepthStencilState(NULL, 0);
		lastCamDesc = cam;
	}

	void ElementSkyBox::DrawShadow(ID3D11DeviceContext* context)
	{
		context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		world = dx::XMMatrixIdentity();

		world = dx::XMMatrixScaling(10000.0f, 10000.0f, 10000.0f) *
			dx::XMMatrixTranslation(dx::XMVectorGetX(lastCamDesc.eye), dx::XMVectorGetY(lastCamDesc.eye), dx::XMVectorGetZ(lastCamDesc.eye));

		UINT stride = sizeof(BJEStruct::VertexPosOnly);
		UINT offset = 0;

		context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		context->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

		BJEStruct::VertexConstantBuffer cb;
		cb.WVP = XMMatrixTranspose(world);
		
		context->UpdateSubresource(BaseElement::GetConstantBuffer(), 0, NULL, &cb, 0, 0);

		context->PSSetShaderResources(0, 1, &ttext->GetTexture());

		context->PSSetSamplers(0, 1, Textures::GetWrapState());

		DepthStencil::SetDepthStencilState(LESS_EQUAL);

		Blend::Get()->DrawNoBlend();
		Blend::Get()->DrawCullStateClockFalse();

		context->DrawIndexed(indices.size(), 0, 0);
		context->OMSetDepthStencilState(NULL, 0);
	}

	std::string ElementSkyBox::GetTextureName()
	{
		std::wstring tmp(ttext->GetTexturePath());

		return std::string(tmp.begin(), tmp.end());
	}

	std::vector<BJEStruct::VertexPosOnly> ElementSkyBox::GetVertices() const
	{
		return vertices;
	}

	std::vector<WORD> ElementSkyBox::GetIndices() const
	{
		return indices;
	}



	ID3D11Buffer* ElementSkyBoxConvertion::cubeBuffer = nullptr;

	ElementSkyBoxConvertion::ElementSkyBoxConvertion()
	{
	}

	ElementSkyBoxConvertion::~ElementSkyBoxConvertion()
	{
	}

	void ElementSkyBoxConvertion::Close()
	{
	}

	void ElementSkyBoxConvertion::Init()
	{
		if (cubeBuffer == nullptr)
			cubeBuffer = Helper::InitConstantBuffer<BJEStruct::CubeGenerateConstantBuffer>(GP::GetDevice());

		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;

		

	}

	void ElementSkyBoxConvertion::Draw(CameraDesc cam, ID3D11DeviceContext* context)
	{
		dx::XMFLOAT4 forward = dx::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		dx::XMFLOAT4 up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		dx::XMFLOAT4 positionCam = dx::XMFLOAT4(dx::XMVectorGetX(cam.eye), dx::XMVectorGetY(cam.eye), dx::XMVectorGetZ(cam.eye), dx::XMVectorGetW(cam.eye));
		dx::XMMATRIX projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);

		matrices.shViewProjection[0] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&positionCam),
			dx::XMLoadFloat4(&positionCam) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		matrices.shViewProjection[1] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&positionCam),
			dx::XMLoadFloat4(&positionCam) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);

		matrices.shViewProjection[2] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&positionCam),
			dx::XMLoadFloat4(&positionCam) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

		matrices.shViewProjection[3] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&positionCam),
			dx::XMLoadFloat4(&positionCam) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		matrices.shViewProjection[4] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&positionCam),
			dx::XMLoadFloat4(&positionCam) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		matrices.shViewProjection[5] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&positionCam),
			dx::XMLoadFloat4(&positionCam) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		GP::BindShader(GP::IRRADIANCE_SKYBOX_TEXTURE);

		
		GP::GetDeviceContext()->GSSetConstantBuffers(0, 1, &cubeBuffer);
		GP::GetDeviceContext()->VSSetConstantBuffers(0, 1, &cubeBuffer);
		GP::GetDeviceContext()->UpdateSubresource(cubeBuffer, 0, NULL, &matrices, 0, 0);

		isDraw = true;
	}

	void ElementSkyBoxConvertion::DrawShadow(ID3D11DeviceContext* context)
	{
	}

	void ElementSkyBoxConvertion::Redraw()
	{
		isDraw = false;
	}

	bool ElementSkyBoxConvertion::IsDraw()
	{
		return isDraw;
	}

}