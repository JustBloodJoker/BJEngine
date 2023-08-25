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
		CLOSE(texture);
		CLOSE(shader);
		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);
		RELEASE(pConstantBuffer);
		RELEASE(wireFrame);
		RELEASE(transparency);
		DELETE(blendFactor);
		RELEASE(renStateCullNone);
	}

	void Object::Draw()
	{

	}

	bool Object::Init()
	{
		return true;
	}

	void Object::SetCamera(Camera*& cam)
	{
		this->cam = cam;
	}

	void Object::SetRastVal(bool choose)
	{
		isRasterized = choose;
	}

	void Object::SetShader(Shader*& shader)
	{
		this->shader = shader;
	}

	void Object::SetTexture(Textures*& texture)
	{
		this->texture = texture; hastext = true;
	}

	void Object::SetTransparency(bool choose, float* blendFactor)
	{
		this->isTransparency = choose;
		this->blendFactor = blendFactor;
	}

	void Object::SetDevice(ID3D11Device*& pd3dDevice)
	{
		this->pd3dDevice = pd3dDevice;
	}

	void Object::SetDeviceContext(ID3D11DeviceContext*& pImmediateContext)
	{
		this->pImmediateContext = pImmediateContext;
	}

	void Object::SetViewAndProjectionMatrix(dx::XMMATRIX view, dx::XMMATRIX projection)
	{
		this->view = view;
		this->projection = projection;
	}

	void Object::SetObjectMatrixPos(float x, float y, float z)
	{
		pos = dx::XMMatrixTranslation(x, y, z);
	}

	void Object::SetObjectMatrixPos(dx::XMFLOAT3 xmf3)
	{
		pos = dx::XMMatrixTranslation(xmf3.x, xmf3.y, xmf3.z);
	}

	void Object::SetObjectMatrixScale(float x, float y, float z)
	{
		scale = dx::XMMatrixScaling(x, y, z);
	}

	void Object::SetObjectMatrixRotationY(float angle)
	{
		rotation = dx::XMMatrixRotationY(angle);
	}

	void Object::SetObjectMatrixRotationX(float angle)
	{
		rotation = dx::XMMatrixRotationX(angle);
	}

	void Object::SetObjectMatrixRotationZ(float angle)
	{
		rotation = dx::XMMatrixRotationZ(angle);
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