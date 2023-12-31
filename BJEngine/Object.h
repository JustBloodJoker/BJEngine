#pragma once
#include "pch.h"
#include "Shader.h"
#include "Textures.h"
#include "Camera.h"
#include "Materials.h"

namespace BJEngine {


	class Object
	{
	private:


	public:

		Object();
		Object(float x, float y, float z);
		~Object();

		virtual void Close();

		virtual void Draw();
		virtual bool Init();

		virtual void MinDraw(dx::XMMATRIX matrix, dx::XMMATRIX mat2);

		struct Vertex
		{
			Vertex() {}
			Vertex(float x, float y, float z,
				float u, float v,
				float nx, float ny, float nz)
				: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

			dx::XMFLOAT3 pos;
			dx::XMFLOAT2 texCoord;
			dx::XMFLOAT3 normal;
		};

		struct ConstantBuffer
		{
			dx::XMMATRIX WVP;
			dx::XMMATRIX World;
			dx::XMMATRIX ViewMatrix;
			dx::XMMATRIX projectionMatrix;
			dx::XMFLOAT4 pos;
			dx::XMMATRIX lView[5];
			dx::XMMATRIX lProj[5];
		};

		template <typename ConstantBufferType>
		static ID3D11Buffer* InitConstantBuffer(ID3D11Device* pd3dDevice);

		template <typename VerticiesType>
		static ID3D11Buffer* InitVertexBuffer(ID3D11Device* pd3dDevice, int sizeInBytes, VerticiesType* tvertices);

		template <typename IndicesType>
		static ID3D11Buffer* InitIndicesBuffer(ID3D11Device* pd3dDevice, int sizeInBytes, IndicesType* tindices);

		void SetCamera(Camera*& cam);
		void SetShader(Shader*& shader);
		void SetTexture(Textures*& texture);
		void SetDevice(ID3D11Device*& pd3dDevice);
		void SetDeviceContext(ID3D11DeviceContext*& pImmediateContext);
		void SetViewAndProjectionMatrix(dx::XMMATRIX view, dx::XMMATRIX projection);

		void SetLightViewAndProjectionMatrix(dx::XMMATRIX view, dx::XMMATRIX projecion, int index);

		virtual bool HasTexture() final { return hastext; };

		void SetObjectMatrixPos(float x, float y, float z);
		void SetObjectMatrixPos(dx::XMFLOAT3 xmf3);
		void SetObjectMatrixScale(float x, float y, float z);
		void SetObjectMatrixRotationY(float angle);
		void SetObjectMatrixRotationX(float angle);
		void SetObjectMatrixRotationZ(float angle);
		void SetTexturesPrefixPath(std::wstring prefPath);

		bool IsInited() { return isInited; }

		dx::XMMATRIX GetObjectMatrix();

	protected:
		bool isInited = false;

		Camera* cam;
		Textures* texture;
		Shader* shader;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11Buffer* pConstantBuffer;

		dx::XMMATRIX world;
		dx::XMMATRIX view;
		dx::XMMATRIX projection;


		dx::XMMATRIX rotation;
		dx::XMMATRIX scale;
		dx::XMMATRIX pos;

		//////////////
		dx::XMMATRIX lView[5];
		dx::XMMATRIX lProjection[5];

		bool hastext = false;

		ID3D11Device* pd3dDevice;
		ID3D11DeviceContext* pImmediateContext;

		std::vector<Materials*> materials;

		dx::BoundingBox objectBox;

		std::wstring texturePrefixPath = L"";
	};

	template<typename ConstantBufferType>
	inline ID3D11Buffer* Object::InitConstantBuffer(ID3D11Device* pd3dDevice)
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

	template<typename VerticiesType>
	inline ID3D11Buffer* BJEngine::Object::InitVertexBuffer(ID3D11Device* pd3dDevice, int sizeInBytes, VerticiesType* tvertices)
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

	template<typename IndicesType>
	inline ID3D11Buffer* BJEngine::Object::InitIndicesBuffer(ID3D11Device* pd3dDevice, int sizeInBytes, IndicesType* tindices)
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

}
