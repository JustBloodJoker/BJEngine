#pragma once
#include "pch.h"
#include "Script.h"
#include "Camera.h"
#include "Shader.h"
#include "Textures.h"
#include "Blend.h"
#include "Materials.h"

namespace BJEngine {


	class Object
	{
	public:

		Object();
		Object(float x, float y, float z);
		~Object();

		virtual void Close();

		virtual void Draw();
		virtual bool Init();

		virtual void MinDraw();

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

		
		void SetCamera(Camera*& cam);
		void SetShader(Shader*& shader);
		void SetTexture(Textures* texture);
		void SetDevice(ID3D11Device*& pd3dDevice);
		void SetDeviceContext(ID3D11DeviceContext*& pImmediateContext);
		
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
		Shader* glowShader;
		Script* script;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11Buffer* pConstantBuffer;

		dx::XMMATRIX world;

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

}
