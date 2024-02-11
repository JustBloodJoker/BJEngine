#include "CameraCone.h"
#include "Blend.h"

namespace BJEngine
{


	CameraCone::CameraCone()
	{
		Init();
	}
	
	bool CameraCone::Init()
	{
		D3D11_INPUT_ELEMENT_DESC layout[1] = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };

		HRESULT hr = S_OK;

		if (shader != nullptr)
			shader->Close();

		shader = new Shader(L"shaders\\SimpleVSPS.hlsl", L"shaders\\SimpleVSPS.hlsl", "VS", "PS");
		
		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init();

		world = dx::XMMatrixIdentity() * dx::XMMatrixScaling(100.0f, 100.0f, 100.0f);

		pVertexBuffer = Helper::InitVertexBuffer(GP::GetDevice(), sizeof(CVertex) * vertices.size(), &vertices[0]);
		pIndexBuffer = Helper::InitIndicesBuffer(GP::GetDevice(), sizeof(WORD) * indices.size(), &indices[0]);
		pConstantBuffer = Helper::InitConstantBuffer<CameraCone::ConstantBuffer>(GP::GetDevice());

		//vertices.clear();
		//indices.clear();

		Log::Get()->Debug("CameraCone is inited");
		
		return true;
	}

	void CameraCone::Draw(dx::XMMATRIX view, dx::XMMATRIX projection,
																 float x , float y , float z, float ya, float pit, float FoV)
	{
		UINT stride = sizeof(CVertex);
		UINT offset = 0;
		GP::GetDeviceContext()->IASetInputLayout(shader->GetInputLayout());
		GP::GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		GP::GetDeviceContext()->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		world = dx::XMMatrixIdentity()  * dx::XMMatrixScaling(FoV, FoV, 1.0f)  * dx::XMMatrixRotationX(pit) * dx::XMMatrixRotationY(ya) * dx::XMMatrixTranslation(x, y, z);


		ConstantBuffer cb;
		cb.WVP = dx::XMMatrixTranspose(world * view * projection);

		GP::GetDeviceContext()->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
		GP::GetDeviceContext()->VSSetConstantBuffers(0, 1, &pConstantBuffer);

		Blend::Get()->DrawWireFrameCullState();

		GP::GetDeviceContext()->VSSetShader(shader->GetVertexShader(), NULL, 0);
		GP::GetDeviceContext()->PSSetShader(shader->GetPixelShader(), NULL, 0);
		GP::GetDeviceContext()->DrawIndexed(indices.size(), 0, 0);

		Blend::Get()->DrawCullNoneState();
	}

	void CameraCone::Close()
	{
		RELEASE(pVertexBuffer);
		RELEASE(pIndexBuffer);
		RELEASE(pConstantBuffer);
		CLOSE(shader); 
		vertices.clear();
		indices.clear();
	}


}