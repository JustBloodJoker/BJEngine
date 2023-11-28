#include "pch.h"
#include "Shader.h"

namespace BJEngine {

	Shader::Shader()
	{
		EPPS = nullptr;
		EPVS = nullptr;
		pVertexLayout = nullptr;
		pVertexShader = nullptr;
		pPixelShader = nullptr;
		pixelShaderName = nullptr;
		vertexShaderName = nullptr;
	}

	Shader::Shader(const wchar_t* pixelShaderName, const wchar_t* vertexShaderName,
		const char* EPVS, const char* EPPS) : EPPS(EPPS), EPVS(EPVS),
		pixelShaderName(pixelShaderName),
		vertexShaderName(vertexShaderName)
	{
		pVertexLayout = nullptr;
		pVertexShader = nullptr;
		pPixelShader = nullptr;
	}

	Shader::~Shader()
	{

	}

	void Shader::Close()
	{
		if (!isClosed)
		{
			RELEASE(pVertexLayout);
			RELEASE(pPixelShader);
			RELEASE(pVertexShader);
			Log::Get()->Debug("Shader was closed");
		}
	}

	bool Shader::Init(ID3D11Device* device)
	{
		HRESULT hr = S_OK;
		ID3DBlob* pVSBlob = NULL;

		hr = m_compileshaderfromfile(vertexShaderName, EPVS, "vs_4_0", &pVSBlob);
		if (FAILED(hr))
		{
			Log::Get()->Err("vertex shader compile error");
			return false;
		}

		hr = device->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pVertexShader);
		if (FAILED(hr))
		{
			Log::Get()->Err("create vertex shader error");
			return false;
		}

		hr = device->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pVertexLayout);
		if (FAILED(hr))
		{
			Log::Get()->Err("create IL error");
			return false;
		}

		ID3DBlob* pPSBlob = NULL;
		hr = m_compileshaderfromfile(pixelShaderName, EPPS, "ps_4_0", &pPSBlob);
		if (FAILED(hr))
		{
			Log::Get()->Err("pixel shader compile error");
			return false;
		}

		hr = device->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pPixelShader);
		RELEASE(pPSBlob);
		if (FAILED(hr))
		{
			Log::Get()->Err("pixel shader create error");
			return false;
		}

		isClosed = false;

		return true;
	}

	HRESULT Shader::m_compileshaderfromfile(const wchar_t* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut)
	{
		HRESULT hr = S_OK;

		DWORD ShaderFlags = D3DCOMPILE_ENABLE_STRICTNESS;
#if defined( DEBUG ) || defined( _DEBUG )
		ShaderFlags |= D3DCOMPILE_DEBUG;
#endif

		ID3DBlob* pErrorBlob;
		hr = D3DX11CompileFromFile(FileName, NULL, NULL, EntryPoint, ShaderModel, ShaderFlags, 0, NULL, ppBlobOut, &pErrorBlob, NULL);
		if (FAILED(hr) && pErrorBlob != NULL)
			OutputDebugStringA((char*)pErrorBlob->GetBufferPointer());

		RELEASE(pErrorBlob);
		return hr;
	}

}