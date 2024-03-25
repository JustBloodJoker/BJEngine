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
			shadowCBuffer = Helper::InitConstantBuffer<BJEStruct::CubeGenerateConstantBuffer>(GP::GetDevice());
			
		depthStencil->InitView(SHADOW_WIDTH, SHADOW_WIDTH,false, 6, DXGI_FORMAT_D32_FLOAT, D3D11_RESOURCE_MISC_TEXTURECUBE, D3D11_BIND_DEPTH_STENCIL, true, true);

		isInited = true;
		
		vp.Width = static_cast<float>(SHADOW_WIDTH);
		vp.Height = static_cast<float>(SHADOW_HEIGHT);
		vp.TopLeftX = 0.0f;
		vp.TopLeftY = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.MinDepth = 0.0f;

	}

	void OmnidirectionalShadow::GenerateView(const BJEStruct::LightDesc ld)
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
		GP::GetDeviceContext()->PSSetShaderResources(SHADOWCUBE_DEFPASS_TEXTURE_POS + deltIndex, 1, &depthStencil->GetSRV());
	}

	void OmnidirectionalShadow::Close()
	{
		CLOSE(depthStencil);
		RELEASE(shadowCBuffer);
	}

	void OmnidirectionalShadow::Draw()
	{
		
		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		GP::BindShader(GP::OMNIDIRECTIONAL_SHADOW_SHADER);

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

		GP::BindShader(GP::SIMPLE_SHADOW_SHADER);

		Blend::Get()->DrawCullFrontState();

		GP::GetDeviceContext()->VSSetConstantBuffers(0, 1, &shadowCBuffer);
		GP::GetDeviceContext()->UpdateSubresource(shadowCBuffer, 0, NULL, &matrices, 0, 0);

		GP::GetDeviceContext()->OMSetRenderTargets(0, nullptr, depthStencil->GetDepthStencil());
		depthStencil->ClearDepthStencilView();
	}

	void SimpleShadow::GenerateView(const BJEStruct::LightDesc ld)
	{
		dx::XMFLOAT4 up = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 0.0f);

		dx::XMMATRIX projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, 1.0f, 1.0f, 10000.0f);

		matrices.shViewProjection = dx::XMMatrixTranspose(dx::XMMatrixLookAtLH(dx::XMLoadFloat4(&ld.pos),
			dx::XMLoadFloat4(&ld.pos) + dx::XMLoadFloat4(&ld.dir),
			dx::XMLoadFloat4(&up)) * projectionMatrix);
	}

	void SimpleShadow::BindSRV(int deltIndex)
	{
		GP::GetDeviceContext()->PSSetShaderResources(SHADOWCUBE_DEFPASS_TEXTURE_POS + MAX_SHADOW_NUM + deltIndex, 1, &depthStencil->GetSRV());
	}

	void SimpleShadow::Close()
	{
		CLOSE(depthStencil);
		RELEASE(shadowCBuffer);
	}

}