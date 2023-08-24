#include "pch.h"
#include "Cube.h"

namespace BJEngine {


	Cube::Cube(CubeDesc desc)
	{
		this->vertices = desc.vertices;
		this->indices = desc.indices;
	}

	Cube::Cube()
	{
		CubeDesc* desc = new CubeDesc();

		this->vertices = desc->vertices;
		this->indices = desc->indices;
	}

	Cube::~Cube()
	{

	}

	void Cube::Close()
	{
		CLOSE(texture);
		CLOSE(shader);
		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);
		RELEASE(pConstantBuffer);
		RELEASE(wireFrame);
		RELEASE(transparency);
		DELETE(blendFactor);
		DELETE(vertices);
	}

	void Cube::Draw()
	{
		DrawRasterized();
		DrawTransparency();

		world = dx::XMMatrixIdentity();
		world = rotation * scale * pos;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		pImmediateContext->IASetInputLayout(shader->GetInputLayout());
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		ConstantBuffer cb;
		cb.WVP = XMMatrixTranspose(world * view * projection);
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

		pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
		pImmediateContext->PSSetShader(shader->GetPixelShader(), NULL, 0);
		pImmediateContext->DrawIndexed(36, 0, 0);

		
	}

	bool Cube::Init()
	{
		HRESULT hr = S_OK;

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		};

		if(shader == nullptr)
			shader = new BJEngine::Shader(L"shaders\\cubeShader.txt", L"shaders\\cubeShader.txt", "VS", "PS");

		if (shader == nullptr)
		{
			Log::Get()->Err("shader cube error");
			return false;
		}

		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init(pd3dDevice);

		pVertexBuffer = Object::InitVertexBuffer(pd3dDevice, sizeof(Vertex) * 24, vertices);
		pIndexBuffer = Object::InitIndicesBuffer(pd3dDevice, sizeof(WORD) * 36, indices);
		pConstantBuffer = Object::InitConstantBuffer<ConstantBuffer>(pd3dDevice);

		if (FAILED(IsRasterizedObj()))
			return false;

		if (hastext) {
			if (texture->InitTextures(pd3dDevice))
				return false;
		}
		if (FAILED(IsTransparencyObj()))
			return false;

		rotation = dx::XMMatrixRotationY(0.0f);
		scale = dx::XMMatrixScaling(1.0f, 1.0f, 1.0f);
		pos = dx::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		return true;
	}


}
