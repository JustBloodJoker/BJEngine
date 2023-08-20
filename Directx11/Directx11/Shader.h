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

		void SetInputLayout(D3D11_INPUT_ELEMENT_DESC* tempLayout, UINT elementsNum) { layout = tempLayout; numElements = elementsNum; }

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

		UINT numElements;
		D3D11_INPUT_ELEMENT_DESC* layout;
		

		const char* EPVS;
		const char* EPPS;
	};


}