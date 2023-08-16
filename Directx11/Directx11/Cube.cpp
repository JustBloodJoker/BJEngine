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
		LCLOSE(directionalLight);
		LCLOSE(pointLight);
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

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		pImmediateContext->IASetInputLayout(shader->GetInputLayout());
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		if (directionalLight != nullptr) {
			directionalLight->DrawLight(pImmediateContext);
		}

		if (pointLight != nullptr) {
			pointLight->DrawLight(pImmediateContext);
		}

		ConstantBuffer cb;
		cb.WVP = XMMatrixTranspose(world * view * projection);
		cb.World = world;
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

		if (hastext) {
			pImmediateContext->PSSetShaderResources(0, 1, &texture->GetTexture());
			pImmediateContext->PSSetSamplers(0, 1, &texture->GetTexSamplerState());
		}

		pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
		pImmediateContext->PSSetShader(shader->GetPixelShader(), NULL, 0);
		pImmediateContext->DrawIndexed(36, 0, 0);
	}

	bool Cube::Init()
	{
		HRESULT hr = S_OK;

		shader->Init(pd3dDevice);

		if (directionalLight != nullptr) {
			directionalLight->InitLight(pd3dDevice);
		}

		if (pointLight != nullptr) {
			pointLight->InitLight(pd3dDevice);
		}

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Vertex) * 24;
		bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bd.CPUAccessFlags = 0;
		D3D11_SUBRESOURCE_DATA InitData;
		ZeroMemory(&InitData, sizeof(InitData));
		InitData.pSysMem = vertices;
		hr = pd3dDevice->CreateBuffer(&bd, &InitData, &pVertexBuffer);
		if (FAILED(hr))
		{
			Log::Get()->Err("vertex buffer create error");
			return false;
		}

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(WORD) * 36;
		bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bd.CPUAccessFlags = 0;
		InitData.pSysMem = indices;
		hr = pd3dDevice->CreateBuffer(&bd, &InitData, &pIndexBuffer);
		if (FAILED(hr))
		{
			Log::Get()->Err("index buffer create error");
			return false;
		}

		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(ConstantBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;
		hr = pd3dDevice->CreateBuffer(&bd, NULL, &pConstantBuffer);
		if (FAILED(hr))
		{
			Log::Get()->Err("constant buffer create error");
			return false;
		}

		if (FAILED(IsRasterizedObj()))
			return false;

		if (hastext) {
			if (texture->InitTextures(pd3dDevice))
				return false;
		}
		if (FAILED(IsTransparencyObj()))
			return false;

		return true;
	}


}
