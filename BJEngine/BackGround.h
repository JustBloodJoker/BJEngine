#pragma once
#include "Object.h"

namespace BJEngine {


	class BackGround : public Object
	{
	public:

		BackGround();
		~BackGround();

		bool Init() override;
		void Draw() override;
		void Close() override;



	private:
		ID3D11DepthStencilState* depthStateLessEqual;

		int latLines = 10, longLines = 10;
		int numSphereVertices = ((latLines - 2) * longLines) + 2;
		int numSphereFaces = ((latLines - 3) * (longLines) * 2) + (longLines * 2);

		std::vector<BJEStruct::VertexBackGround> vertices;
		std::vector<DWORD> indices;

		void CreateSphere();


	};


}