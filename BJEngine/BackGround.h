#pragma once
#include "Object.h"

namespace BJEngine {


	class BackGround : public Object
	{
	public:

		BackGround();
		~BackGround();

		bool Init() override;
		void Draw(const CameraDesc cam) override;
		void Close() override;

		void SetTexture(Textures* texture);

	private:
		
		bool hastext = false;
		Textures* texture;
		
		int latLines = 10, longLines = 10;
		int numSphereVertices = ((latLines - 2) * longLines) + 2;
		int numSphereFaces = ((latLines - 3) * (longLines) * 2) + (longLines * 2);

		std::vector<BJEStruct::VertexBackGround> vertices;
		std::vector<DWORD> indices;

		void CreateSphere();


	};


}