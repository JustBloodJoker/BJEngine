#include "pch.h"
#include "Map.h"

namespace BJEngine {

	Map::Map()
	{
		MapDesc* md = new MapDesc();
		this->vertices = md->vertices;
		this->indices = md->indices;
	}

	Map::~Map()
	{
	}

	bool Map::Init()
	{
		HRESULT hr = S_OK;

		D3D11_INPUT_ELEMENT_DESC layout[3] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init(pd3dDevice);

		if(light != nullptr)
		light->InitLight(pd3dDevice);

		InitIsLightConstantBuffer();

		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(Vertex) * 4;
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
		bd.ByteWidth = sizeof(WORD) * 6;
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

		ZeroMemory(&cmdesc, sizeof(D3D11_RASTERIZER_DESC));
		cmdesc.FillMode = D3D11_FILL_SOLID;
		cmdesc.FrontCounterClockwise = true;
		cmdesc.CullMode = D3D11_CULL_NONE;
		hr = pd3dDevice->CreateRasterizerState(&cmdesc, &renStateCullNone);
		if (FAILED(hr))
		{
			Log::Get()->Err("Create rast state error");
			return false;
		}

		rotation = dx::XMMatrixRotationY(0.0f);
		scale = dx::XMMatrixScaling(500.0f, 10.0f, 500.0f);
		pos = dx::XMMatrixTranslation(0.0f, 10.0f, 0.0f);

		return true;
	}

	void Map::Draw()
	{
		DrawRasterized();
		DrawTransparency();

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		pImmediateContext->IASetInputLayout(shader->GetInputLayout());
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		DrawIsLightConstantBuffer();

		if (light != nullptr) {
			light->DrawLight(pImmediateContext);
		}
		
		world = rotation *  scale * pos;

		ConstantBuffer cb;
		cb.WVP = XMMatrixTranspose(world * view * projection);
		cb.World = world;
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

		if (hastext) {
			pImmediateContext->PSSetShaderResources(0, 1, &texture->GetTexture());
			pImmediateContext->PSSetSamplers(0, 1, &texture->GetTexSamplerState());
		}
		pImmediateContext->RSSetState(renStateCullNone);
		pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
		pImmediateContext->PSSetShader(shader->GetPixelShader(), NULL, 0);
		pImmediateContext->DrawIndexed(6, 0, 0);

	}

}