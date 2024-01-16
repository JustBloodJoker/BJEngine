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
		CameraCone(ID3D11Device* pd3dDevice);

		bool Init(ID3D11Device* pd3dDevice);
		void Draw(ID3D11DeviceContext* pImmediateContext, dx::XMMATRIX view, dx::XMMATRIX projection, float x, float y, float z, float ya, float pit, float FoV);
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
			CVertex(-BJEUtils::GetWindowWidth() / (20 * M_PI / 2),  -BJEUtils::GetWindowHeight() / (20 * M_PI / 2),   0.0f),
			CVertex( BJEUtils::GetWindowWidth() / (20 * M_PI / 2),  -BJEUtils::GetWindowHeight() / (20 * M_PI / 2),   0.0f),
			CVertex( BJEUtils::GetWindowWidth() / (20 * M_PI / 2),   BJEUtils::GetWindowHeight() / (20 * M_PI / 2),   0.0f),
			CVertex(-BJEUtils::GetWindowWidth() / (20 * M_PI / 2),   BJEUtils::GetWindowHeight() / (20 * M_PI / 2),   0.0f),
			CVertex( 0.0f,   0.0f,  -BJEUtils::GetWindowHeight() / 20)
		

				/*CVertex(-50.0f, -50.0f, 0.0f),
				CVertex(50.0f, -50.0f, 0.0f),
				CVertex(50.0f, 50.0f, 0.0f),
				CVertex(-50.0f, 50.0f, 0.0f),
				CVertex(0.0f, 0.0f, -50.0f)*/

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