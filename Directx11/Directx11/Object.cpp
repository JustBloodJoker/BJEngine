#include "pch.h"
#include "Object.h"

namespace BJEngine {


	Object::Object()
	{
		world = dx::XMMatrixIdentity();

		pVertexBuffer = nullptr;
		pIndexBuffer = nullptr;
		pConstantBuffer = nullptr;
	}

	Object::Object(float x, float y, float z)
	{
		pVertexBuffer = nullptr;
		pIndexBuffer = nullptr;
		pConstantBuffer = nullptr;
	}

	Object::~Object()
	{

	}

	void Object::Close()
	{
		LCLOSE(light);
		CLOSE(texture);
		CLOSE(shader);
		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);
		RELEASE(pConstantBuffer);
		RELEASE(wireFrame);
		RELEASE(transparency);
		DELETE(blendFactor);
		RELEASE(ilcb);
		RELEASE(renStateCullNone);
	}

	void Object::Draw()
	{

	}

	bool Object::Init()
	{
		return true;
	}

	bool Object::InitIsLightConstantBuffer()
	{
		D3D11_BUFFER_DESC bd;
		ZeroMemory(&bd, sizeof(bd));
		bd.Usage = D3D11_USAGE_DEFAULT;
		bd.ByteWidth = sizeof(IsLightsConstantBuffer);
		bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bd.CPUAccessFlags = 0;

		HRESULT hr = pd3dDevice->CreateBuffer(&bd, NULL, &ilcb);
		if (FAILED(hr)) {
			Log::Get()->Err("IslightConstantBuffer create error");
			return false;
		}

		return true;

	}

	void Object::DrawIsLightConstantBuffer()
	{
		pImmediateContext->UpdateSubresource(ilcb, 0, NULL, &istypeoflight, 0, 0);
		pImmediateContext->PSSetConstantBuffers(1, 1, &ilcb);

	}

	void Object::SetCamera(Camera* cam)
	{
		this->cam = cam;
	}

	void Object::SetRastVal(bool choose)
	{
		isRasterized = choose;
	}

	void Object::SetShader(Shader* shader)
	{
		this->shader = shader;
	}

	void Object::SetTexture(Textures* texture)
	{
		this->texture = texture; hastext = true;
	}

	void Object::SetTransparency(bool choose, float* blendFactor)
	{
		this->isTransparency = choose;
		this->blendFactor = blendFactor;
	}

	void Object::SetDirectionLight(DirectionalLightDesc* lightdesc)
	{
		if (light != nullptr) {
			ZeroMemory(&istypeoflight, sizeof(IsLightsConstantBuffer));
			LCLOSE(light);
		}

		light = new DirectionalLight(lightdesc);
		istypeoflight.isDirLight = true;
	}

	void Object::SetPointLight(PointLightDesc* lightdesc)
	{
		if (light != nullptr) {
			ZeroMemory(&istypeoflight, sizeof(IsLightsConstantBuffer));
			LCLOSE(light);
		}

		light = new PointLight(lightdesc);
		istypeoflight.isPointLight = true;
	}

	void Object::SetSpotLight(SpotLightDesc* lightdesc)
	{
		if (light != nullptr) {
			ZeroMemory(&istypeoflight, sizeof(IsLightsConstantBuffer));
			LCLOSE(light);
		}

		light = new SpotLight(lightdesc);
		istypeoflight.isSpotLight = true;
	}

	void Object::SetDevice(ID3D11Device* pd3dDevice)
	{
		this->pd3dDevice = pd3dDevice;
	}

	void Object::SetDeviceContext(ID3D11DeviceContext* pImmediateContext)
	{
		this->pImmediateContext = pImmediateContext;
	}

	void Object::SetViewAndProjectionMatrix(dx::XMMATRIX view, dx::XMMATRIX projection)
	{
		this->view = view;
		this->projection = projection;
	}

	HRESULT Object::IsRasterizedObj()
	{
		if (isRasterized) {
			ZeroMemory(&wfdesc, sizeof(D3D11_RASTERIZER_DESC));
			wfdesc.FillMode = D3D11_FILL_WIREFRAME;
			wfdesc.CullMode = D3D11_CULL_NONE;
			HRESULT hr = pd3dDevice->CreateRasterizerState(&wfdesc, &wireFrame);

			return hr;
		}

		return S_OK;
	}

	void Object::DrawRasterized()
	{
		if (isRasterized) {
			pImmediateContext->RSSetState(wireFrame);
		}
	}

	HRESULT Object::IsTransparencyObj()
	{
		if (isTransparency) {
			D3D11_BLEND_DESC blendDesc;
			ZeroMemory(&blendDesc, sizeof(blendDesc));

			D3D11_RENDER_TARGET_BLEND_DESC rtbd;
			ZeroMemory(&rtbd, sizeof(rtbd));

			rtbd.BlendEnable = true;
			rtbd.SrcBlend = D3D11_BLEND_SRC_COLOR;
			rtbd.DestBlend = D3D11_BLEND_BLEND_FACTOR;
			rtbd.BlendOp = D3D11_BLEND_OP_ADD;
			rtbd.SrcBlendAlpha = D3D11_BLEND_ONE;
			rtbd.DestBlendAlpha = D3D11_BLEND_ZERO;
			rtbd.BlendOpAlpha = D3D11_BLEND_OP_ADD;
			rtbd.RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;

			blendDesc.AlphaToCoverageEnable = false;
			blendDesc.RenderTarget[0] = rtbd;

			if (FAILED(pd3dDevice->CreateBlendState(&blendDesc, &transparency))) return E_FAIL;
		}

		return S_OK;
	}

	void Object::DrawTransparency()
	{
		if (isTransparency) {

			pImmediateContext->OMSetBlendState(transparency, blendFactor, 0xffffffff);
		}
		else {
			pImmediateContext->OMSetBlendState(0, 0, 0xffffffff);
		}

	}


}