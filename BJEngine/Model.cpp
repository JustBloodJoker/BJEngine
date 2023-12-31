#include "pch.h"
#include "Model.h"


namespace BJEngine {

	Model::Model()
	{
	}

	Model::Model(const char* FileName)
	{
		this->filename = FileName;
	}

	Model::~Model()
	{
	}

	bool Model::Init()
	{
		if (!LoadModel())
		{
			return false;
		};

		HRESULT hr = S_OK;

		D3D11_INPUT_ELEMENT_DESC layout[] = {
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",     0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT", 0, DXGI_FORMAT_R32G32B32_FLOAT,    0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		if (shader == nullptr)
		{
			shader = new BJEngine::Shader(L"shaders\\ObjectShaderPS.hlsl", L"shaders\\ObjectShaderVS.hlsl", "VS", "PS");
		}

		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init(pd3dDevice);

		for (int i = 0; i < packVertices.size(); i++)
		{
			packpVertexBuffer.push_back(Object::InitVertexBuffer(pd3dDevice, sizeof(Vertex) * packVertices[i].size(), &packVertices[i][0]));

			packpIndexBuffer.push_back(Object::InitIndicesBuffer(pd3dDevice, sizeof(WORD) * packindices[i].size(), &packindices[i][0]));

		}

		pConstantBuffer = Object::InitConstantBuffer<Object::ConstantBuffer>(pd3dDevice);

		rotation = dx::XMMatrixRotationY(0.0f);
		scale = dx::XMMatrixScaling(1.0f, 1.0f, 1.0f);
		pos = dx::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		world = rotation * scale * pos;

		Log::Get()->Debug("Model is inited");
		isInited = true;

		return true;
	}

	void Model::Draw()
	{
		world = rotation * scale * pos;

		for (auto& el : animation)
		{
			el.SetObjectMatrix(&world);
		}
		if (animation.size())
		{

			animation[0].DrawAnimation();

		}

		dx::XMMATRIX projectionL;
		dx::XMMATRIX viewL;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;
		pImmediateContext->IASetInputLayout(shader->GetInputLayout());

		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (int i = 0; i < packpVertexBuffer.size(); i++)
		{
			objectBox.CreateFromPoints(objectBox, dx::XMVector3Transform(maxExtentLocal[i], world),
				dx::XMVector3Transform(minExtentLocal[i], world)
			);

			if (cam->GetFrustum().Intersects(objectBox) || cam->GetFrustum().Contains(objectBox))
			{
				pImmediateContext->IASetVertexBuffers(0, 1, &packpVertexBuffer[i], &stride, &offset);
				pImmediateContext->IASetIndexBuffer(packpIndexBuffer[i], DXGI_FORMAT_R16_UINT, 0);

				ConstantBuffer cb;
				cb.WVP = XMMatrixTranspose(world * view * projection);
				//cb.World = XMMatrixTranspose(world);
				cb.World = XMMatrixTranspose(world);
				cb.ViewMatrix = cam->GetViewMatrix();
				cb.projectionMatrix = cam->GetProjectionMatrix();

				for (int i = 0; i < sizeof(lView) / sizeof(dx::XMMATRIX); i++)
				{
					cb.lView[i] = XMMatrixTranspose(lView[i]);
					cb.lProj[i] = XMMatrixTranspose(lProjection[i]);
				}

				pImmediateContext->UpdateSubresource(pConstantBuffer, 0, NULL, &cb, 0, 0);
				pImmediateContext->VSSetConstantBuffers(1, 1, &pConstantBuffer);

				materials[packMaterials[i]]->Draw(pImmediateContext, 2, 0, 1, 100);

				Blend::Get()->DrawCullState(pImmediateContext);

				pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
				pImmediateContext->PSSetShader(shader->GetPixelShader(), NULL, 0);
				pImmediateContext->DrawIndexed(packindices[i].size(), 0, 0);
			}
		}

		if (PackMananger::Get()->GetSavingStatus())
		{
			ObjectType* tpe = new ObjectType();
			
			tpe->path = filename;
			tpe->prepath = std::string(texturePrefixPath.begin(), texturePrefixPath.end());
			PackMananger::Get()->AddObject(*tpe);

			delete tpe;
		}

	}

	void Model::Close()
	{
		Object::Close();
		packVertices.clear();
		packindices.clear();
		packMaterials.clear();
		packpVertexBuffer.clear();
		packpIndexBuffer.clear();
		materials.clear();
	}

	void Model::MinDraw(dx::XMMATRIX matrix, dx::XMMATRIX mat2)
	{
		world = rotation * scale * pos;

		UINT stride = sizeof(Vertex);
		UINT offset = 0;

		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (int i = 0; i < packpVertexBuffer.size(); i++)
		{

			pImmediateContext->IASetVertexBuffers(0, 1, &packpVertexBuffer[i], &stride, &offset);
			pImmediateContext->IASetIndexBuffer(packpIndexBuffer[i], DXGI_FORMAT_R16_UINT, 0);



			pImmediateContext->DrawIndexed(packindices[i].size(), 0, 0);

		}
	}

	bool Model::LoadModel()
	{


		scene = importer.ReadFile(filename,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals);
		if (!scene)
			return false;

		if (scene->mNumAnimations != 0)
		{
			for (int i = 0; i < scene->mNumAnimations; i++)
			{
				animation.push_back({});
				animation[i].SetAnimationParam(scene->mAnimations[i], scene->mRootNode);
				nodd = scene->mRootNode;
			}
		}

		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			materials.push_back(new Materials(pd3dDevice));

			aiColor3D tColor = { 0.0f,0.0f,0.0f };

			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, tColor) == AI_SUCCESS;

			float tt = 0.0f;
			scene->mMaterials[i]->Get(AI_MATKEY_TRANSPARENCYFACTOR, tt);

			materials[i]->SetParam(DIFFUSE, dx::XMFLOAT4(tColor.r, tColor.g, tColor.b, tt));

			tColor = { 0,0,0 };
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_SPECULAR, tColor);
			materials[i]->SetParam(SPECULAR, dx::XMFLOAT4(tColor.r, tColor.g, tColor.b, tt));

			tColor = { 0,0,0 };
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_EMISSIVE, tColor);
			materials[i]->SetParam(EMISSIVE, dx::XMFLOAT4(tColor.r, tColor.g, tColor.b, tt));

			tColor = { 0,0,0 };
			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_AMBIENT, tColor);

			float tp = 0.0f;
			scene->mMaterials[i]->Get(AI_MATKEY_SHININESS, tp);

			materials[i]->SetParam(AMBIENT, dx::XMFLOAT4(tColor.r, tColor.g, tColor.b, tt));

			materials[i]->SetParam(SPECULAR_POWER, tp);

			aiString pathname;
			scene->mMaterials[i]->GetTexture(aiTextureType_DIFFUSE, 0, &pathname);

			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname), pd3dDevice);
			}
			pathname.Clear();

			scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_NORMAL_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname), pd3dDevice);
			}
			pathname.Clear();


			scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_ROUGHNESS_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname), pd3dDevice);
			}
			pathname.Clear();
		}

		dx::XMFLOAT3 minExtent;
		dx::XMFLOAT3 maxExtent;



		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[i];

			packVertices.push_back({});
			packindices.push_back({});
			packMaterials.push_back(mesh->mMaterialIndex);

			minExtent.x = mesh->mVertices[0].x;
			minExtent.y = mesh->mVertices[0].y;
			minExtent.z = mesh->mVertices[0].z;
			maxExtent.x = mesh->mVertices[0].x;
			maxExtent.y = mesh->mVertices[0].y;
			maxExtent.z = mesh->mVertices[0].z;

			for (int j = 0; j < mesh->mNumVertices; j++)
			{
				packVertices[i].push_back({});

				packVertices[i][j].pos.x = mesh->mVertices[j].x;
				packVertices[i][j].pos.y = mesh->mVertices[j].y;
				packVertices[i][j].pos.z = mesh->mVertices[j].z;

				minExtent.x = std::min(minExtent.x, mesh->mVertices[j].x);
				minExtent.y = std::min(minExtent.y, mesh->mVertices[j].y);
				minExtent.z = std::min(minExtent.z, mesh->mVertices[j].z);

				maxExtent.x = std::max(maxExtent.x, mesh->mVertices[j].x);
				maxExtent.y = std::max(maxExtent.y, mesh->mVertices[j].y);
				maxExtent.z = std::max(maxExtent.z, mesh->mVertices[j].z);

				if (mesh->HasTextureCoords(0))
				{
					packVertices[i][j].texCoord.x = mesh->mTextureCoords[0][j].x;
					packVertices[i][j].texCoord.y = mesh->mTextureCoords[0][j].y;
				}

				packVertices[i][j].normal.x = mesh->mNormals[j].x;
				packVertices[i][j].normal.y = mesh->mNormals[j].y;
				packVertices[i][j].normal.z = mesh->mNormals[j].z;

				packVertices[i][j].tangent = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
				if (mesh->HasTangentsAndBitangents())
				{
					packVertices[i][j].tangent.x = mesh->mTangents[j].x;
					packVertices[i][j].tangent.y = mesh->mTangents[j].y;
					packVertices[i][j].tangent.z = mesh->mTangents[j].z;
				}

			}

			minExtentLocal.push_back(dx::XMVectorSet(minExtent.x, minExtent.y, minExtent.z, 1.0f));
			maxExtentLocal.push_back(dx::XMVectorSet(maxExtent.x, maxExtent.y, maxExtent.z, 1.0f));

			for (int j = 0; j < mesh->mNumFaces; j++)
			{
				packindices[i].push_back(mesh->mFaces[j].mIndices[0]);
				packindices[i].push_back(mesh->mFaces[j].mIndices[1]);
				packindices[i].push_back(mesh->mFaces[j].mIndices[2]);
			}
			if (!mesh->HasTangentsAndBitangents())
			{
				for (UINT j = 0; j < packindices[i].size(); j += 3)
				{
					UINT i0 = packindices[i][j];
					UINT i1 = packindices[i][j + 1];
					UINT i2 = packindices[i][j + 2];

					const dx::XMFLOAT3& p0 = packVertices[i][i0].pos;
					const dx::XMFLOAT3& p1 = packVertices[i][i1].pos;
					const dx::XMFLOAT3& p2 = packVertices[i][i2].pos;

					const dx::XMFLOAT2& tc0 = packVertices[i][i0].texCoord;
					const dx::XMFLOAT2& tc1 = packVertices[i][i1].texCoord;
					const dx::XMFLOAT2& tc2 = packVertices[i][i2].texCoord;

					dx::XMFLOAT3 edge1(p1.x - p0.x, p1.y - p0.y, p1.z - p0.z);
					dx::XMFLOAT3 edge2(p2.x - p0.x, p2.y - p0.y, p2.z - p0.z);

					dx::XMFLOAT2 deltaUV1(tc1.x - tc0.x, tc1.y - tc0.y);
					dx::XMFLOAT2 deltaUV2(tc2.x - tc0.x, tc2.y - tc0.y);

					float denominator = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
					if (denominator == 0.0f)
						continue;

					float r = 1.0f / denominator;

					dx::XMFLOAT3 tangent(
						(edge1.x * deltaUV2.y - edge2.x * deltaUV1.y) * r,
						(edge1.y * deltaUV2.y - edge2.y * deltaUV1.y) * r,
						(edge1.z * deltaUV2.y - edge2.z * deltaUV1.y) * r
					);

					packVertices[i][i0].tangent = dx::XMFLOAT3(packVertices[i][i0].tangent.x + tangent.x,
						packVertices[i][i0].tangent.y + tangent.y,
						packVertices[i][i0].tangent.z + tangent.z);

					packVertices[i][i1].tangent = dx::XMFLOAT3(packVertices[i][i1].tangent.x + tangent.x,
						packVertices[i][i1].tangent.y + tangent.y,
						packVertices[i][i1].tangent.z + tangent.z);

					packVertices[i][i2].tangent = dx::XMFLOAT3(packVertices[i][i2].tangent.x + tangent.x,
						packVertices[i][i2].tangent.y + tangent.y,
						packVertices[i][i2].tangent.z + tangent.z);
				}
				for (UINT j = 0; j < packVertices[i].size(); ++j)
				{
					dx::XMFLOAT3& tangent = packVertices[i][j].tangent;
					dx::XMVECTOR tangentVector = dx::XMVectorSet(tangent.x, tangent.y, tangent.z, 0.0f);
					tangentVector = XMVector3Normalize(tangentVector);
					XMStoreFloat3(&packVertices[i][j].tangent, tangentVector);
				}
			}
		}
		return true;
	}


	bool Model::InitAnimation()
	{





		return true;
	}



}