#pragma once
#include "pch.h"


namespace BJEngine {


	class Shader
	{
	public:
		Shader();
		Shader(const wchar_t* pixelShaderName,
			const wchar_t* vertexShaderName,
			const char* EPVS,
			const char* EPPS);

		~Shader();

		void Close();

		bool Init(ID3D11Device* device);

		ID3D11InputLayout* GetInputLayout() { return pVertexLayout; }
		ID3D11VertexShader* GetVertexShader() { return pVertexShader; }
		ID3D11PixelShader* GetPixelShader() { return pPixelShader; }

	private:
		HRESULT m_compileshaderfromfile(const wchar_t* FileName, LPCSTR EntryPoint, LPCSTR ShaderModel, ID3DBlob** ppBlobOut);

		ID3D11InputLayout* pVertexLayout;
		ID3D11VertexShader* pVertexShader;
		ID3D11PixelShader* pPixelShader;

		const wchar_t* pixelShaderName;
		const wchar_t* vertexShaderName;

		D3D11_INPUT_ELEMENT_DESC layout[3] =
		{
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		const char* EPVS;
		const char* EPPS;
	};


}