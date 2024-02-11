#pragma once
#include "pch.h"
#include "Shader.h"

namespace BJEngine
{


	class CameraCone
	{
		struct CVertex;
		struct ConstantBuffer;
	public:
		CameraCone();

		bool Init();
		void Draw(dx::XMMATRIX view, dx::XMMATRIX projection, float x, float y, float z, float ya, float pit, float FoV);
		void Close();

	private:

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11Buffer* pConstantBuffer;

		dx::XMMATRIX world;
		Shader* shader;

		struct CVertex
		{
			CVertex() {}
			CVertex(float x, float y, float z) : pos(x, y, z) {}

			dx::XMFLOAT3 pos;
		};

		struct ConstantBuffer
		{
			dx::XMMATRIX WVP;
		};

		
		std::vector<CVertex> vertices =
		{
			CVertex( 1920.0f / (20 * M_PI / 2),  -1080.0f / (20 * M_PI / 2),   0.0f),
			CVertex( 1920.0f / (20 * M_PI / 2),  -1080.0f / (20 * M_PI / 2),   0.0f),
			CVertex( 1920.0f / (20 * M_PI / 2),   1080.0f / (20 * M_PI / 2),   0.0f),
			CVertex(-1920.0f / (20 * M_PI / 2),   1080.0f / (20 * M_PI / 2),   0.0f),
			CVertex( 0.0f,   0.0f,  -1080.0f / 20)
		};
	
		std::vector<WORD> indices = 
		{
			0, 1, 2,
			0, 2, 3,

			0, 4, 1,
			1, 4, 2,
			2, 4, 3,
			3, 4, 0
		};
	};


}