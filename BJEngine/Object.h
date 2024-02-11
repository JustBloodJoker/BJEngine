#pragma once
#include "pch.h"
#include "Script.h"
#include "Camera.h"
#include "Textures.h"
#include "Blend.h"
#include "Materials.h"
#include "Element.h"

namespace BJEngine {

	class Materials;

	class Object
	{
	public:

		Object();
		Object(float x, float y, float z);
		~Object();

		virtual void Close();

		virtual void Draw(const CameraDesc cam);
		virtual bool Init();

		virtual void MinDraw(dx::BoundingFrustum frustum);

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

		

		
		
		void SetLightViewAndProjectionMatrix(dx::XMMATRIX view, dx::XMMATRIX projecion, int index);

		

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

		
		Script* script;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		std::vector<ID3D11Buffer*> ConstantBuffers;

		dx::XMMATRIX world;

		dx::XMMATRIX rotation;
		dx::XMMATRIX scale;
		dx::XMMATRIX pos;

		//////////////
		dx::XMMATRIX lView[5];
		dx::XMMATRIX lProjection[5];

		

		std::vector<Materials*> materials;

		

		std::wstring texturePrefixPath = L"";
	};

}
