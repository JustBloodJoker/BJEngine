#include "pch.h"
#include "Shadow.h"

namespace BJEngine
{

	void Shadow::InitShadow(ID3D11Device* pd3dDevice, int shadowType)
	{
		switch (shadowType)
		{
		case BJEUtils::POINTLIGHT:
			InitCubeMapShadows(pd3dDevice);
			break;
		case BJEUtils::SPOTLIGHT: case BJEUtils::DIRECTIONALLIGHT:
			InitSimpleMapShadows(pd3dDevice);

		}
	}

	void Shadow::Render(ID3D11DeviceContext* pImmediateContext, LightDesc light, std::vector<Object*> objects)
	{
		if (light.lightType == BJEUtils::POINTLIGHT)
		{
			RenderCubeMapShadows(pImmediateContext, dx::XMFLOAT3(light.pos.x, light.pos.y, light.pos.z), objects);
		}
		else
		{
			RenderSimpleMapShadows(pImmediateContext, dx::XMFLOAT3(light.pos.x, light.pos.y, light.pos.z),
				dx::XMFLOAT3(light.dir.x, light.dir.y, light.dir.z), objects);
		}
	}



	void Shadow::Close()
	{

	}

	ID3D11ShaderResourceView* const* Shadow::GetTexture()
	{
		return &srv;
	}

	void Shadow::InitCubeMapShadows(ID3D11Device* pd3dDevice)
	{
		HRESULT hr;

		pDepthConstantBuffer = Object::InitConstantBuffer<ConstantDepthBuffer>(pd3dDevice);

		D3D11_TEXTURE2D_DESC textDesc = {};
		ID3D11Texture2D* textureSRV;
		textDesc.Width = SHADOW_WIDTH;
		textDesc.Height = SHADOW_HEIGHT;
		textDesc.MipLevels = 1;
		textDesc.ArraySize = 6;
		textDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		textDesc.SampleDesc.Count = 1;
		textDesc.SampleDesc.Quality = 0;
		textDesc.Usage = D3D11_USAGE_DEFAULT;
		textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		textDesc.CPUAccessFlags = 0;
		textDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

		hr = pd3dDevice->CreateTexture2D(&textDesc, nullptr, &textureSRV);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;

		hr = pd3dDevice->CreateShaderResourceView(textureSRV, &srvDesc, &srv);

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D32_FLOAT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.ArraySize = 1;
		for (int i = 0; i < 6; i++)
		{
			descView.Texture2DArray.FirstArraySlice = i;
			hr = pd3dDevice->CreateDepthStencilView(textureSRV, &descView, &dsv[i]);
		}

		shader = new Shader(L"shadowPS.hlsl", L"shadowVS.hlsl", "VS", "PS");
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init(pd3dDevice);

		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;



	}

	void Shadow::RenderCubeMapShadows(ID3D11DeviceContext* pImmediateContext, dx::XMFLOAT3  pos, std::vector<Object*> objects)
	{
		dx::XMMATRIX projection;
		dx::XMMATRIX view[6];

		projection = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);

		dx::XMFLOAT4 directionalCoord = dx::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		dx::XMFLOAT4 upCoord = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		view[0] = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&directionalCoord),
			dx::XMLoadFloat4(&upCoord));

		directionalCoord = dx::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
		upCoord = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		view[1] = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&directionalCoord),
			dx::XMLoadFloat4(&upCoord));

		directionalCoord = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		upCoord = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);

		view[2] = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&directionalCoord),
			dx::XMLoadFloat4(&upCoord));

		directionalCoord = dx::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		upCoord = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

		view[3] = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&directionalCoord),
			dx::XMLoadFloat4(&upCoord));

		directionalCoord = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
		upCoord = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		view[4] = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&directionalCoord),
			dx::XMLoadFloat4(&upCoord));

		directionalCoord = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
		upCoord = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		view[5] = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&directionalCoord),
			dx::XMLoadFloat4(&upCoord));

		pImmediateContext->RSSetViewports(1, &vp);


		pImmediateContext->IASetInputLayout(shader->GetInputLayout());
		pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
		pImmediateContext->PSSetShader(nullptr, NULL, 0);

		for (int x = 0; x < 6; x++)
		{
			pImmediateContext->OMSetRenderTargets(0, nullptr, dsv[x]);
			pImmediateContext->ClearDepthStencilView(dsv[x], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			for (auto& el : objects)
			{
				depthBuffer.WVP = dx::XMMatrixTranspose(el->GetObjectMatrix() * view[x] * projection);
				pImmediateContext->UpdateSubresource(pDepthConstantBuffer, 0, NULL, &depthBuffer, 0, 0);
				pImmediateContext->VSSetConstantBuffers(1, 1, &pDepthConstantBuffer);
				el->MinDraw(view[x], projection);
			}
		}
	}


	void Shadow::InitSimpleMapShadows(ID3D11Device* pd3dDevice)
	{
		HRESULT hr;

		pDepthConstantBuffer = Object::InitConstantBuffer<ConstantDepthBuffer>(pd3dDevice);

		D3D11_TEXTURE2D_DESC textDesc = {};
		ID3D11Texture2D* textureSRV;
		textDesc.Width = SHADOW_WIDTH;
		textDesc.Height = SHADOW_HEIGHT;
		textDesc.MipLevels = 1;
		textDesc.ArraySize = 1;
		textDesc.Format = DXGI_FORMAT_R32_TYPELESS;
		textDesc.SampleDesc.Count = 1;
		textDesc.SampleDesc.Quality = 0;
		textDesc.Usage = D3D11_USAGE_DEFAULT;
		textDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
		textDesc.CPUAccessFlags = 0;
		textDesc.MiscFlags = 0;

		hr = pd3dDevice->CreateTexture2D(&textDesc, nullptr, &textureSRV);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = pd3dDevice->CreateShaderResourceView(textureSRV, &srvDesc, &srv);

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D32_FLOAT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;
		hr = pd3dDevice->CreateDepthStencilView(textureSRV, &descView, &dsv[0]);

		shader = new Shader(L"shadowPS.hlsl", L"shadowVS.hlsl", "VS", "PS");
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init(pd3dDevice);

		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;


	}

	void Shadow::RenderSimpleMapShadows(ID3D11DeviceContext* pImmediateContext, dx::XMFLOAT3  pos, dx::XMFLOAT3 dir, std::vector<Object*> objects)
	{
		prj = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);

		dx::XMFLOAT4 upCoord = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		vw = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat3(&dir),
			dx::XMLoadFloat4(&upCoord));

		pImmediateContext->RSSetViewports(1, &vp);
		pImmediateContext->IASetInputLayout(shader->GetInputLayout());
		pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
		pImmediateContext->PSSetShader(nullptr, NULL, 0);

		pImmediateContext->OMSetRenderTargets(0, nullptr, dsv[0]);
		pImmediateContext->ClearDepthStencilView(dsv[0], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		for (auto& el : objects)
		{
			depthBuffer.WVP = dx::XMMatrixTranspose(el->GetObjectMatrix() * vw * prj);
			pImmediateContext->UpdateSubresource(pDepthConstantBuffer, 0, NULL, &depthBuffer, 0, 0);
			pImmediateContext->VSSetConstantBuffers(1, 1, &pDepthConstantBuffer);
			el->MinDraw(vw, prj);
		}

	}

}