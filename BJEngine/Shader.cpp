#include "Shader.h"

namespace BJEngine {

	Shader::Shader()
	{
		EPPS = nullptr;
		EPVS = nullptr;
		pVertexLayout = nullptr;
		pVertexShader = nullptr;
		pPixelShader = nullptr;
		pGeometryShader = nullptr;
		pixelShaderName = nullptr;
		vertexShaderName = nullptr;
	}

	Shader::Shader(const wchar_t* pixelShaderName, const wchar_t* vertexShaderName,
		const char* EPVS, const char* EPPS) : EPPS(EPPS), EPVS(EPVS),
		pixelShaderName(pixelShaderName),
		vertexShaderName(vertexShaderName),
		EPGS("") , geometryShaderName(L"")
	{
		pVertexLayout = nullptr;
		pVertexShader = nullptr;
		pPixelShader = nullptr;
		pGeometryShader = nullptr;
	}

	Shader::Shader(const wchar_t* pixelShaderName, const wchar_t* vertexShaderName, const wchar_t* geometryShaderName,
		const char* EPGS, const char* EPVS, const char* EPPS) :EPPS(EPPS), EPVS(EPVS), EPGS(EPGS),
		pixelShaderName(pixelShaderName),
		vertexShaderName(vertexShaderName), 
		geometryShaderName(geometryShaderName)
	{
		pVertexLayout = nullptr;
		pVertexShader = nullptr;
		pPixelShader = nullptr;
		pGeometryShader = nullptr;
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

	bool Shader::Init()
	{
		HRESULT hr = S_OK;
		
		if (vertexShaderName != L"" || EPVS != "" || layout != nullptr)
		{
			ID3DBlob* pVSBlob = NULL;
			hr = m_compileshaderfromfile(vertexShaderName, EPVS, "vs_5_0", &pVSBlob);
			if (FAILED(hr))
			{
				Log::Get()->Err("vertex shader compile error");
				return false;
			}

			hr = GP::GetDevice()->CreateVertexShader(pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), NULL, &pVertexShader);
			if (FAILED(hr))
			{
				Log::Get()->Err("create vertex shader error");
				return false;
			}

			hr = GP::GetDevice()->CreateInputLayout(layout, numElements, pVSBlob->GetBufferPointer(), pVSBlob->GetBufferSize(), &pVertexLayout);
			if (FAILED(hr))
			{
				Log::Get()->Err("create IL error");
				return false;
			}
		}
		
		if (EPPS != "")
		{
			ID3DBlob* pPSBlob = NULL;
			hr = m_compileshaderfromfile(pixelShaderName, EPPS, "ps_5_0", &pPSBlob);
			if (FAILED(hr))
			{
				Log::Get()->Err("pixel shader compile error");
				return false;
			}

			hr = GP::GetDevice()->CreatePixelShader(pPSBlob->GetBufferPointer(), pPSBlob->GetBufferSize(), NULL, &pPixelShader);
			RELEASE(pPSBlob);
			if (FAILED(hr))
			{
				Log::Get()->Err("pixel shader create error");
				return false;
			}
		}

		if (EPGS != "")
		{
			ID3DBlob* pGSBlob = NULL;
			hr = m_compileshaderfromfile(geometryShaderName, EPGS, "gs_5_0", &pGSBlob);
			if (FAILED(hr))
			{
				Log::Get()->Err("geometry shader compile error");
				return false;
			}

			hr = GP::GetDevice()->CreateGeometryShader(pGSBlob->GetBufferPointer(), pGSBlob->GetBufferSize(), NULL, &pGeometryShader);
			RELEASE(pGSBlob);
			if (FAILED(hr))
			{
				Log::Get()->Err("geometry shader create error");
				return false;
			}
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