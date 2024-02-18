#include "GP.h"
#include "Shader.h"

ID3D11Device* pDevice = nullptr;
ID3D11DeviceContext* pDeviceContext = nullptr;
std::unordered_map<GP::SHADER_TYPE, BJEngine::Shader*> shaders;

ID3D11Device*& GP::GetDevice()
{
    return pDevice;
}

ID3D11DeviceContext*& GP::GetDeviceContext()
{
    return pDeviceContext;
}

void GP::BindShader(SHADER_TYPE type)
{
	GP::GetDeviceContext()->IASetInputLayout(shaders[type]->GetInputLayout());
	GP::GetDeviceContext()->VSSetShader(shaders[type]->GetVertexShader(), NULL, 0);
	GP::GetDeviceContext()->PSSetShader(shaders[type]->GetPixelShader(), NULL, 0);
	GP::GetDeviceContext()->GSSetShader(shaders[type]->GetGeometryShader(), NULL, 0);
}

bool GP::InitShaders()
{
	if (shaders[MODEL_SHADER] == nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = {
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
				{ "BITANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		shaders[MODEL_SHADER] = new BJEngine::Shader(L"shaders\\ObjectShaderPS.hlsl", L"shaders\\ObjectShaderVS.hlsl", L"shaders\\ObjectShaderGS.hlsl", "GS", "VS", "PS");;
		shaders[MODEL_SHADER]->SetInputLayout(layout, ARRAYSIZE(layout));
		shaders[MODEL_SHADER]->Init();
	}
	if (shaders[GLOW_MODEL_SHADER] == nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 } };
		shaders[GLOW_MODEL_SHADER] = new BJEngine::Shader(L"shaders\\GlowPS.hlsl", L"shaders\\GlowVS.hlsl", "VS", "PS");
		shaders[GLOW_MODEL_SHADER]->SetInputLayout(layout, ARRAYSIZE(layout));
		shaders[GLOW_MODEL_SHADER]->Init();
	}
	if (shaders[BACKGROUND_SHADER] == nullptr)
	{
		D3D11_INPUT_ELEMENT_DESC layout[] = { { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
											  { "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 } };
		shaders[BACKGROUND_SHADER] = new BJEngine::Shader(L"shaders\\CubeMapShader.txt", L"shaders\\CubeMapShader.txt", "SKYMAP_VS", "SKYMAP_PS");
		shaders[BACKGROUND_SHADER]->SetInputLayout(layout, ARRAYSIZE(layout));
		shaders[BACKGROUND_SHADER]->Init();
	}
	
	if (shaders[DEFFEREDSCENE_SHADER_PS] == nullptr)
	{
		shaders[DEFFEREDSCENE_SHADER_PS] = new BJEngine::Shader(L"shaders\\DefferedScenePS.hlsl", L"", "", "PS");
		shaders[DEFFEREDSCENE_SHADER_PS]->Init();
	}
	
		

		









    return true;
}

bool GP::ClearGlobalParameters()
{
	for (auto &el : shaders)
	{
		CLOSE(el.second);
	}


	RELEASE(GP::GetDevice());
	if (GP::GetDeviceContext())
		GP::GetDeviceContext()->ClearState();
	RELEASE(GP::GetDeviceContext());


	return true;
}
