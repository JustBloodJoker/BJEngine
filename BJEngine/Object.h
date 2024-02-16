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
	class Element;

	class Object
	{
	public:

		Object();
		Object(float x, float y, float z);
		~Object();

		virtual void Close();

		virtual void Draw(const CameraDesc cam);
		virtual bool Init();

		
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

		
		void SetTexturesPrefixPath(std::wstring prefPath);

		bool IsInited() { return isInited; }

		std::vector<Element*>& MoveElements() { return elements; };

	protected:
		bool isInited = false;

		
		Script* script;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11Buffer* ConstantBuffers;


		std::vector<Materials*> materials;
		std::vector<Element*> elements;

		std::wstring texturePrefixPath = L"";
	};

}
