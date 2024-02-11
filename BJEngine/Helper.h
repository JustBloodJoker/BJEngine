#pragma once 
#include "pch.h"

namespace BJEngine
{



	class Log;

	class Helper
	{
	public:

		template <typename ConstantBufferType>
		static ID3D11Buffer* InitConstantBuffer(ID3D11Device* pd3dDevice)
		{
			HRESULT hr = S_OK;
			ID3D11Buffer* tpConstantBuffer;
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeof(ConstantBufferType);
			if (bd.ByteWidth % 16 != 0)
			{
				Log::Get()->Debug("ByteWidth in constant buffer are incorrect!");
			}
			bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bd.CPUAccessFlags = 0;
			hr = pd3dDevice->CreateBuffer(&bd, NULL, &tpConstantBuffer);
			if (FAILED(hr))
			{
				Log::Get()->Err("constant buffer create error");
				return nullptr;
			}

			return tpConstantBuffer;
		}

		template <typename VerticiesType>
		static ID3D11Buffer* InitVertexBuffer(ID3D11Device* pd3dDevice, int sizeInBytes, VerticiesType* tvertices)
		{
			HRESULT hr = S_OK;
			ID3D11Buffer* tpVertexBuffer;
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeInBytes;
			bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bd.CPUAccessFlags = 0;
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			InitData.pSysMem = tvertices;
			hr = pd3dDevice->CreateBuffer(&bd, &InitData, &tpVertexBuffer);
			if (FAILED(hr))
			{
				Log::Get()->Err("vertex buffer create error");
				return nullptr;
			}
			return tpVertexBuffer;
		}

		template <typename IndicesType>
		static ID3D11Buffer* InitIndicesBuffer(ID3D11Device* pd3dDevice, int sizeInBytes, IndicesType* tindices)
		{
			HRESULT hr = S_OK;
			ID3D11Buffer* tpIndexBuffer;
			D3D11_BUFFER_DESC bd;
			ZeroMemory(&bd, sizeof(bd));
			D3D11_SUBRESOURCE_DATA InitData;
			ZeroMemory(&InitData, sizeof(InitData));
			bd.Usage = D3D11_USAGE_DEFAULT;
			bd.ByteWidth = sizeInBytes;
			bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
			bd.CPUAccessFlags = 0;
			InitData.pSysMem = tindices;
			hr = pd3dDevice->CreateBuffer(&bd, &InitData, &tpIndexBuffer);
			if (FAILED(hr))
			{
				Log::Get()->Err("index buffer create error");
				return nullptr;
			}

			return tpIndexBuffer;
		}

	};


}