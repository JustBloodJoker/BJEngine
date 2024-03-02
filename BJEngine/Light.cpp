#include "Light.h"
#include "Blend.h"

namespace BJEngine {


	ID3D11Buffer* OmnidirectionalShadow::shadowCBuffer = nullptr;
	ID3D11Buffer* SimpleShadow::shadowCBuffer = nullptr;

	OmnidirectionalShadow::OmnidirectionalShadow()
	{
		Init();
	}

	void OmnidirectionalShadow::Init()
	{
		depthStencil = new DepthStencil();
		
		
		if(shadowCBuffer == nullptr)
			shadowCBuffer = Helper::InitConstantBuffer<OmnidirectionalShadowConstantBuffer>(GP::GetDevice());
			
		depthStencil->InitView(SHADOW_WIDTH, SHADOW_WIDTH,false, 6, DXGI_FORMAT_D32_FLOAT, D3D11_RESOURCE_MISC_TEXTURECUBE, D3D11_BIND_DEPTH_STENCIL, true, true);

		D3D11_INPUT_ELEMENT_DESC layout[] = 
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		shader = new Shader(L"shaders\\OmnidirectionalShadow.hlsl",L"shaders\\OmnidirectionalShadow.hlsl",L"shaders\\OmnidirectionalShadow.hlsl", "GS","VS","PS");
		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init();

		isInited = true;
		
		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;
	}

	void OmnidirectionalShadow::GenerateView(const LightDesc ld)
	{
		
		dx::XMFLOAT4 forward = dx::XMFLOAT4(1.0f, 0.0f, 0.0f, 0.0f);
		dx::XMFLOAT4 up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		dx::XMMATRIX projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);

		matrices.shViewProjection[0] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
			dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(-1.0f, 0.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		matrices.shViewProjection[1] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
			dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);

		matrices.shViewProjection[2] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
				dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&forward),
				dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, -1.0f, 0.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);

		matrices.shViewProjection[3] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
			dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		matrices.shViewProjection[4] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
			dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);

		forward = dx::XMFLOAT4(0.0f, 0.0f, -1.0f, 0.0f);
		up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		matrices.shViewProjection[5] = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
			dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&forward),
			dx::XMLoadFloat4(&up)) * projectionMatrix);
	
	}

	void OmnidirectionalShadow::BindSRV(int deltIndex)
	{
		GP::GetDeviceContext()->PSSetShaderResources(SHADOWCUBE_TEXTURE_POS + deltIndex, 1, &depthStencil->GetSRV());
	}

	void OmnidirectionalShadow::Close()
	{
		CLOSE(shader);
		CLOSE(depthStencil);
		RELEASE(shadowCBuffer);
	}

	void OmnidirectionalShadow::Draw()
	{
		
		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		GP::GetDeviceContext()->IASetInputLayout(shader->GetInputLayout());
		GP::GetDeviceContext()->VSSetShader(shader->GetVertexShader(), NULL, 0);
		GP::GetDeviceContext()->PSSetShader(shader->GetPixelShader(), NULL, 0);
		GP::GetDeviceContext()->GSSetShader(shader->GetGeometryShader(), NULL, 0);

		Blend::Get()->DrawCullBackState();
		
		GP::GetDeviceContext()->GSSetConstantBuffers(0, 1, &shadowCBuffer);
		GP::GetDeviceContext()->VSSetConstantBuffers(0, 1, &shadowCBuffer);
		GP::GetDeviceContext()->UpdateSubresource(shadowCBuffer, 0, NULL, &matrices, 0, 0);
		
		GP::GetDeviceContext()->OMSetRenderTargets(0, nullptr, depthStencil->GetDepthStencil());
		depthStencil->ClearDepthStencilView();
		
	}

	

	SimpleShadow::SimpleShadow()
	{
		Init();
	}

	void SimpleShadow::Init()
	{
		depthStencil = new DepthStencil();


		if (shadowCBuffer == nullptr)
			shadowCBuffer = Helper::InitConstantBuffer<SimpleShadowConstantBuffer>(GP::GetDevice());

		depthStencil->InitView(SHADOW_WIDTH, SHADOW_WIDTH, false, 1, DXGI_FORMAT_D32_FLOAT, 0U, D3D11_BIND_DEPTH_STENCIL, true, true);

		D3D11_INPUT_ELEMENT_DESC layout[] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }
		};
		shader = new Shader(L"shaders\\SimpleShadowMap.hlsl", L"shaders\\SimpleShadowMap.hlsl", "VS", "PS");
		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init();

		isInited = true;

		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;
	}

	void SimpleShadow::Draw()
	{
		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		GP::GetDeviceContext()->IASetInputLayout(shader->GetInputLayout());
		GP::GetDeviceContext()->VSSetShader(shader->GetVertexShader(), NULL, 0);
		GP::GetDeviceContext()->PSSetShader(shader->GetPixelShader(), NULL, 0);
		GP::GetDeviceContext()->GSSetShader(shader->GetGeometryShader(), NULL, 0);

		Blend::Get()->DrawCullFrontState();

		GP::GetDeviceContext()->VSSetConstantBuffers(0, 1, &shadowCBuffer);
		GP::GetDeviceContext()->UpdateSubresource(shadowCBuffer, 0, NULL, &matrices, 0, 0);

		GP::GetDeviceContext()->OMSetRenderTargets(0, nullptr, depthStencil->GetDepthStencil());
		depthStencil->ClearDepthStencilView();
	}

	void SimpleShadow::GenerateView(const LightDesc ld)
	{
		dx::XMFLOAT4 up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		dx::XMMATRIX projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);

		matrices.shViewProjection = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
			dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&ld.dir),
			dx::XMLoadFloat4(&up)) * projectionMatrix);
	}

	void SimpleShadow::BindSRV(int deltIndex)
	{
		GP::GetDeviceContext()->PSSetShaderResources(SHADOWCUBE_TEXTURE_POS + MAX_SHADOW_NUM + deltIndex, 1, &depthStencil->GetSRV());
	}

	void SimpleShadow::Close()
	{
		CLOSE(shader);
		CLOSE(depthStencil);
		RELEASE(shadowCBuffer);
	}

}