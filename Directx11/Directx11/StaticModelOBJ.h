#pragma once
#include "pch.h"
#include "Object.h"

namespace BJEngine {
	
	
	class StaticModelOBJ :
		public Object
	{
	public:
		StaticModelOBJ();
		StaticModelOBJ(std::wstring objFileName);
		~StaticModelOBJ();

		bool Init() override;
		void Draw() override;
		void Close() override;

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
			dx::XMFLOAT3 tangent;
		};

		struct VertexDesc
		{
			std::vector<Vertex> vertices;
			std::wstring mtlName;
			int vectorsize;

		};
		
		struct MaterialDesc
		{
			std::wstring textKDName;
			std::wstring textBumpName;
			dx::XMFLOAT4 difColor;
			bool transparent;
			Textures* texture;
			Textures* textureBump;
		};

	private:
		
		bool LoadObjModel();

		std::map<std::wstring, MaterialDesc> mat;
		std::vector<VertexDesc> packVertices;
		
		struct Obj {

			std::wstring nameObject;
			std::wstring matName;

			std::vector<MaterialDesc> material;
			std::vector<DWORD> indices;

			std::vector<dx::XMFLOAT3> vertPos;
			std::vector<dx::XMFLOAT3> vertNorm;
			std::vector<dx::XMFLOAT2> vertTexCoord;


			std::vector<int> vertPosIndex;
			std::vector<int> vertNormIndex;
			std::vector<int> vertTCIndex;

			bool hasText;
			bool hasNorm;
			int smooth;
		};
		std::vector<Obj> objs;

		std::vector<ID3D11Buffer*> packpVertexBuffer;
		std::vector<ID3D11Buffer*> packpIndexBuffer;

		std::wstring filename;
		std::wstring mtlfilename;

		std::wifstream file;

		ID3D11BlendState* Transparency;
	};


}