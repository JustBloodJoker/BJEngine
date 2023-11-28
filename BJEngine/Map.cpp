#include "pch.h"
#include "Map.h"


#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>


namespace BJEngine {


	Map::Map()
	{
		MapDesc* md = new MapDesc();

		for (size_t i = 0; i < sizeof(md->vertices) / sizeof(Vertex); i++)
			this->vertices.push_back(md->vertices[i]);

		for (size_t i = 0; i < sizeof(md->indices) / sizeof(WORD); i++)
			this->indices.push_back(md->indices[i]);
	}

	Map::~Map()
	{
	}

	bool Map::Init()
	{
		HRESULT hr = S_OK;

		if (isTerrain)
			InitTerrainTexture();

		D3D11_INPUT_ELEMENT_DESC layout[4] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};

		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init(pd3dDevice);

		pVertexBuffer = Object::InitVertexBuffer(pd3dDevice, sizeof(Vertex) * vertices.size(), &vertices[0]);
		pIndexBuffer = Object::InitIndicesBuffer(pd3dDevice, sizeof(WORD) * indices.size(), &indices[0]);
		pConstantBuffer = Object::InitConstantBuffer<Object::ConstantBuffer>(pd3dDevice);

		materials.push_back(new Materials(pd3dDevice));
		materials[0]->SetParam(DIFFUSE, dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));
		materials[0]->SetParam(AMBIENT, dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f));

		if (hastext) {
			materials[0]->SetTexture(HAS_TEXTURE, texture, pd3dDevice);
		}

		rotation = dx::XMMatrixRotationY(0.0f);
		scale = dx::XMMatrixScaling(1.0f, 1.0f, 1.0f);
		pos = dx::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		Log::Get()->Debug("Map is inited");
		isInited = true;

		return true;
	}

	void Map::Draw()
	{

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		pImmediateContext->IASetInputLayout(shader->GetInputLayout());
		pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);
		pImmediateContext->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R16_UINT, 0);
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		world = rotation * scale * pos;

		materials[0]->Draw(pImmediateContext, 2, 0, 1, 100);

		ConstantBuffer cb;
		cb.WVP = XMMatrixTranspose(world * view * projection);
		cb.World = world;
		cb.ViewMatrix = cam->GetViewMatrix();
		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);

		Blend::Get()->DrawNoBlend(pImmediateContext);
		Blend::Get()->DrawCullState(pImmediateContext);
		pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
		pImmediateContext->PSSetShader(shader->GetPixelShader(), NULL, 0);
		pImmediateContext->DrawIndexed(indices.size(), 0, 0);

	}

	void Map::Close()
	{
		Object::Close();
		vertices.clear();
		indices.clear();
	}

	void Map::SetTerrainTexture(const char* filename)
	{
		this->terrainFileName = filename;
		isTerrain = true;
	}

	void Map::InitTerrainTexture()
	{
		FILE* filePtr;
		BITMAPFILEHEADER bitmapFileHeader;
		BITMAPINFOHEADER bitmapInfoHeader;
		int imageSize, index;
		unsigned char height;

		filePtr = fopen(terrainFileName, "rb");
		if (filePtr == NULL)
			return;

		fread(&bitmapFileHeader, sizeof(BITMAPFILEHEADER), 1, filePtr);
		fread(&bitmapInfoHeader, sizeof(BITMAPINFOHEADER), 1, filePtr);

		hmInfo.terrainWidth = bitmapInfoHeader.biWidth;
		hmInfo.terrainHeight = bitmapInfoHeader.biHeight;

		imageSize = hmInfo.terrainWidth * hmInfo.terrainHeight * 3;

		unsigned char* bitmapImage = new unsigned char[imageSize];

		fseek(filePtr, bitmapFileHeader.bfOffBits, SEEK_SET);

		fread(bitmapImage, 1, imageSize, filePtr);

		fclose(filePtr);

		hmInfo.heightMap = new dx::XMFLOAT3[hmInfo.terrainWidth * hmInfo.terrainHeight];

		int k = 0;

		float heightFactor = 10.0f;

		for (int j = 0; j < hmInfo.terrainHeight; j++)
		{
			for (int i = 0; i < hmInfo.terrainWidth; i++)
			{
				height = bitmapImage[k];

				index = (hmInfo.terrainHeight * j) + i;

				hmInfo.heightMap[index].x = (float)i;
				hmInfo.heightMap[index].y = (float)height / heightFactor;
				hmInfo.heightMap[index].z = (float)j;

				k += 3;
			}
		}

		delete[] bitmapImage;
		bitmapImage = nullptr;

		int cols = hmInfo.terrainWidth;
		int rows = hmInfo.terrainHeight;

		vertices.clear();
		vertices.resize(rows * cols);

		indices.clear();
		indices.resize(((rows - 1) * (cols - 1) * 2) * 3);

		for (DWORD i = 0; i < rows; ++i)
		{
			for (DWORD j = 0; j < cols; ++j)
			{
				vertices[i * cols + j].pos = hmInfo.heightMap[i * cols + j];
				vertices[i * cols + j].normal = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
			}
		}

		k = 0;
		int texUIndex = 0;
		int texVIndex = 0;
		for (DWORD i = 0; i < rows - 1; i++)
		{
			for (DWORD j = 0; j < cols - 1; j++)
			{
				indices[k] = i * cols + j;
				vertices[i * cols + j].texCoord = dx::XMFLOAT2(texUIndex + 0.0f, texVIndex + 1.0f);

				indices[k + 1] = i * cols + j + 1;
				vertices[i * cols + j + 1].texCoord = dx::XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

				indices[k + 2] = (i + 1) * cols + j;
				vertices[(i + 1) * cols + j].texCoord = dx::XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);


				indices[k + 3] = (i + 1) * cols + j;
				vertices[(i + 1) * cols + j].texCoord = dx::XMFLOAT2(texUIndex + 0.0f, texVIndex + 0.0f);

				indices[k + 4] = i * cols + j + 1;
				vertices[i * cols + j + 1].texCoord = dx::XMFLOAT2(texUIndex + 1.0f, texVIndex + 1.0f);

				indices[k + 5] = (i + 1) * cols + j + 1;
				vertices[(i + 1) * cols + j + 1].texCoord = dx::XMFLOAT2(texUIndex + 1.0f, texVIndex + 0.0f);

				k += 6;

				texUIndex++;
			}
			texUIndex = 0;
			texVIndex++;
		}

		for (size_t i = 0; i < indices.size(); i += 3)
		{
			UINT index0 = indices[i];
			UINT index1 = indices[i + 1];
			UINT index2 = indices[i + 2];

			dx::XMFLOAT3 v0 = vertices[index0].pos;
			dx::XMFLOAT3 v1 = vertices[index1].pos;
			dx::XMFLOAT3 v2 = vertices[index2].pos;

			dx::XMFLOAT3 edge1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
			dx::XMFLOAT3 edge2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);

			dx::XMFLOAT3 normal;
			dx::XMStoreFloat3(&normal, dx::XMVector3Normalize(dx::XMVector3Cross(dx::XMLoadFloat3(&edge1), dx::XMLoadFloat3(&edge2))));

			vertices[index0].normal = normal;
			vertices[index1].normal = normal;
			vertices[index2].normal = normal;
		}

		for (Vertex& vertex : vertices)
		{
			dx::XMStoreFloat3(&vertex.normal, dx::XMVector3Normalize(dx::XMLoadFloat3(&vertex.normal)));
		}
	}


}