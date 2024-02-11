#include "Shadow.h"
#include "Camera.h"

namespace BJEngine
{

	void Shadow::InitShadow(int shadowType)
	{
		switch (shadowType)
		{
		case BJEUtils::POINTLIGHT:
			InitCubeMapShadows();
			break;
		case BJEUtils::SPOTLIGHT: case BJEUtils::DIRECTIONALLIGHT:
			InitSimpleMapShadows();

		}
	}

	void Shadow::Render(LightDesc light, std::vector<Object*> objects)
	{
		if (light.lightType == BJEUtils::POINTLIGHT)
		{
			RenderCubeMapShadows(dx::XMFLOAT3(light.pos.x, light.pos.y, light.pos.z), objects);
		}
		else
		{
			RenderSimpleMapShadows(dx::XMFLOAT3(light.pos.x, light.pos.y, light.pos.z),
				dx::XMFLOAT3(light.dir.x, light.dir.y, light.dir.z), objects);
		}
	}

	void Shadow::Close()
	{
		RELEASE(srv);
		for (size_t index = 0; index < 6; index++)
		{
			RELEASE(dsv[index]);
		}
		RELEASE(pDepthConstantBuffer);
	}

	ID3D11ShaderResourceView* const* Shadow::GetTexture()
	{
		return &srv;
	}

	void Shadow::InitCubeMapShadows()
	{
		HRESULT hr;

		pDepthConstantBuffer = Helper::InitConstantBuffer<ConstantDepthBuffer>(GP::GetDevice());

		D3D11_TEXTURE2D_DESC textDesc = {};
		
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

		hr = GP::GetDevice()->CreateTexture2D(&textDesc, nullptr, &textureSRV);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.TextureCube.MipLevels = 1;

		hr = GP::GetDevice()->CreateShaderResourceView(textureSRV, &srvDesc, &srv);

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D32_FLOAT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2DARRAY;
		descView.Texture2DArray.MipSlice = 0;
		descView.Texture2DArray.ArraySize = 1;
		for (int i = 0; i < 6; i++)
		{
			descView.Texture2DArray.FirstArraySlice = i;
			hr = GP::GetDevice()->CreateDepthStencilView(textureSRV, &descView, &dsv[i]);
		}

		shader = new Shader(L"shadowPS.hlsl", L"shadowVS.hlsl", "VS", "PS");
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init();

		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;



	}

	void Shadow::RenderCubeMapShadows(dx::XMFLOAT3  pos, std::vector<Object*> objects)
	{

		dx::XMFLOAT4 forward = dx::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		dx::XMFLOAT4 up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		camDesc[0].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up));

		forward = dx::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		camDesc[1].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up));

		forward = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);

		camDesc[2].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up));
	
		forward = dx::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

		camDesc[3].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up));
		
		forward = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		camDesc[4].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up));
		
		forward = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		camDesc[5].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
			dx::XMLoadFloat3(&pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up));

		camDesc[0].projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);
		camDesc[1].projectionMatrix = camDesc[0].projectionMatrix;
		camDesc[2].projectionMatrix = camDesc[0].projectionMatrix;
		camDesc[3].projectionMatrix = camDesc[0].projectionMatrix;
		camDesc[4].projectionMatrix = camDesc[0].projectionMatrix;
		camDesc[5].projectionMatrix = camDesc[0].projectionMatrix;

		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		GP::GetDeviceContext()->IASetInputLayout(shader->GetInputLayout());
		GP::GetDeviceContext()->VSSetShader(shader->GetVertexShader(), NULL, 0);
		GP::GetDeviceContext()->PSSetShader(shader->GetPixelShader(), NULL, 0);
		//GP::GetDeviceContext()->PSSetShader(nullptr, NULL, 0);
		GP::GetDeviceContext()->GSSetShader(nullptr, NULL, 0);


		Blend::Get()->DrawCullBackState();

		for (int x = 0; x < 6; x++)
		{
			camDesc[x].GenFrustum();
			
			GP::GetDeviceContext()->OMSetRenderTargets(0, nullptr, dsv[x]);
			GP::GetDeviceContext()->ClearDepthStencilView(dsv[x], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
			for (auto& el : objects)
			{
				depthBuffer.WVP = dx::XMMatrixTranspose(el->GetObjectMatrix() * camDesc[x].viewMatrix * camDesc[x].projectionMatrix);
				depthBuffer.pos = pos;
				GP::GetDeviceContext()->UpdateSubresource(pDepthConstantBuffer, 0, NULL, &depthBuffer, 0, 0);
				GP::GetDeviceContext()->VSSetConstantBuffers(1, 1, &pDepthConstantBuffer);
				GP::GetDeviceContext()->PSSetConstantBuffers(1, 1, &pDepthConstantBuffer);
				el->MinDraw(camDesc[x].frustum);
			}
		}

		if(Input::Get()->CheckKeyState(DIK_7))
			D3DX11SaveTextureToFile(GP::GetDeviceContext(), textureSRV, D3DX11_IFF_DDS, L"SHADOW.dds");
	}

	void Shadow::InitSimpleMapShadows()
	{
		HRESULT hr;

		pDepthConstantBuffer = Helper::InitConstantBuffer<ConstantDepthBuffer>(GP::GetDevice());

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

		hr = GP::GetDevice()->CreateTexture2D(&textDesc, nullptr, &textureSRV);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R32_FLOAT;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MipLevels = 1;

		hr = GP::GetDevice()->CreateShaderResourceView(textureSRV, &srvDesc, &srv);

		D3D11_DEPTH_STENCIL_VIEW_DESC descView = {};
		descView.Format = DXGI_FORMAT_D32_FLOAT;
		descView.Flags = 0;
		descView.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		descView.Texture2D.MipSlice = 0;
		hr = GP::GetDevice()->CreateDepthStencilView(textureSRV, &descView, &dsv[0]);

		shader = new Shader(L"shadowPS.hlsl", L"shadowVS.hlsl", "VS", "PS");
		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init();

		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;


	}

	void Shadow::RenderSimpleMapShadows(dx::XMFLOAT3  pos, dx::XMFLOAT3 dir, std::vector<Object*> objects)
	{
		camDesc[0].projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);
		camDesc[0].viewMatrix = dx::XMMatrixLookAtLH(dx::XMLoadFloat3(&pos),
								dx::XMLoadFloat3(&pos) + dx::XMLoadFloat3(&dir),
								dx::XMLoadFloat4(new dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f)));
	
		GP::GetDeviceContext()->RSSetViewports(1, &vp);
		GP::GetDeviceContext()->IASetInputLayout(shader->GetInputLayout());
		GP::GetDeviceContext()->VSSetShader(shader->GetVertexShader(), NULL, 0);
		GP::GetDeviceContext()->PSSetShader(nullptr, NULL, 0);
		GP::GetDeviceContext()->GSSetShader(nullptr, NULL, 0);

		GP::GetDeviceContext()->OMSetRenderTargets(0, nullptr, dsv[0]);
		GP::GetDeviceContext()->ClearDepthStencilView(dsv[0], D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		
		//Blend::Get()->DrawCullBackState();
		
		for (auto& el : objects)
		{
			camDesc[0].GenFrustum();

			depthBuffer.WVP = dx::XMMatrixTranspose(el->GetObjectMatrix() * camDesc[0].viewMatrix * camDesc[0].projectionMatrix);
			GP::GetDeviceContext()->UpdateSubresource(pDepthConstantBuffer, 0, NULL, &depthBuffer, 0, 0);
			GP::GetDeviceContext()->VSSetConstantBuffers(1, 1, &pDepthConstantBuffer);
			el->MinDraw(camDesc[0].frustum);
		}
	}

}