#include "Model.h"
#include "UI.h"

namespace BJEngine 
{


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

		Log::Get()->Debug("Model was inited");
		isInited = true;
		
		if(!script)
			script = new Script();

		

		
		return true;
	}

	void Model::Draw(const CameraDesc cam)
	{
		if (PackMananger::Get()->GetSavingStatus())
		{
			ObjectType* tpe = new ObjectType();
			
			tpe->path = filename;
			tpe->prepath = std::string(texturePrefixPath.begin(), texturePrefixPath.end());
			tpe->script = script->GetName();
			PackMananger::Get()->AddObject(*tpe);

			delete tpe;
		}
	}

	void Model::Close()
	{
		Object::Close();
		materials.clear();
	}

	void Model::SetScript(std::string data)
	{
		script = new Script();
		script->ParseScript(_strdup(data.c_str()));
	}

	bool Model::LoadModel()
	{
		std::vector<BJEStruct::ModelVertex> ver;
		std::vector<WORD> ind;
		dx::XMVECTOR min;
		dx::XMVECTOR max;

		const aiScene* scene = nullptr;
		Assimp::Importer importer;
		
		scene = importer.ReadFile(filename,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (!scene)
			return false;

		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			materials.push_back(new Materials());

			aiColor3D tColor = { 0.0f,0.0f,0.0f };

			scene->mMaterials[i]->Get(AI_MATKEY_COLOR_DIFFUSE, tColor) == AI_SUCCESS;

			float tt = 0.0f;
			scene->mMaterials[i]->Get(AI_MATKEY_OPACITY, tt);

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
				materials[i]->SetTexture(HAS_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname));
			}
			pathname.Clear();

			scene->mMaterials[i]->GetTexture(aiTextureType_NORMALS, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_NORMAL_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname));
			}
			pathname.Clear();


			scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_DIFFUSE_ROUGHNESS, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_ROUGHNESS_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname));
			}
			pathname.Clear();
			
			scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_EMISSIVE, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_EMISSION_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname));
			}
			pathname.Clear();

			scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_SPECULAR_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname));
			}
			pathname.Clear();

			scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_HEIGHT, 0, &pathname);
			if (pathname.length != 0)
			{
				int ii;
				ii = 0;
			}
			pathname.Clear();
		}

		dx::XMFLOAT3 minExtent;
		dx::XMFLOAT3 maxExtent;

		int numofIndex = 0;

		for (int i = 0; i < scene->mNumMeshes; i++)
		{
			const aiMesh* mesh = scene->mMeshes[i];

			minExtent.x = mesh->mVertices[0].x;
			minExtent.y = mesh->mVertices[0].y;
			minExtent.z = mesh->mVertices[0].z;
			maxExtent.x = mesh->mVertices[0].x;
			maxExtent.y = mesh->mVertices[0].y;
			maxExtent.z = mesh->mVertices[0].z;

			for (int j = 0; j < mesh->mNumVertices; j++)
			{
				ver.push_back({});

				ver[j].pos.x = mesh->mVertices[j].x;
				ver[j].pos.y = mesh->mVertices[j].y;
				ver[j].pos.z = mesh->mVertices[j].z;

				minExtent.x = std::min(minExtent.x, mesh->mVertices[j].x);
				minExtent.y = std::min(minExtent.y, mesh->mVertices[j].y);
				minExtent.z = std::min(minExtent.z, mesh->mVertices[j].z);

				maxExtent.x = std::max(maxExtent.x, mesh->mVertices[j].x);
				maxExtent.y = std::max(maxExtent.y, mesh->mVertices[j].y);
				maxExtent.z = std::max(maxExtent.z, mesh->mVertices[j].z);

				if (mesh->HasTextureCoords(0))
				{
					ver[j].texCoord.x = mesh->mTextureCoords[0][j].x;
					ver[j].texCoord.y = mesh->mTextureCoords[0][j].y;
				}

				ver[j].normal.x = mesh->mNormals[j].x;
				ver[j].normal.y = mesh->mNormals[j].y;
				ver[j].normal.z = mesh->mNormals[j].z;

				ver[j].tangent = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
				if (mesh->HasTangentsAndBitangents())
				{
					ver[j].tangent.x = mesh->mTangents[j].x;
					ver[j].tangent.y = mesh->mTangents[j].y;
					ver[j].tangent.z = mesh->mTangents[j].z;

					ver[j].bitangent.x = mesh->mBitangents[j].x;
					ver[j].bitangent.y = mesh->mBitangents[j].y;
					ver[j].bitangent.z = mesh->mBitangents[j].z;
				}

			}
			min = dx::XMVectorSet(minExtent.x, minExtent.y, minExtent.z, 1.0f);
			max = dx::XMVectorSet(maxExtent.x, maxExtent.y, maxExtent.z, 1.0f);
			
			for (int j = 0; j < mesh->mNumFaces; j++)
			{
				ind.push_back(mesh->mFaces[j].mIndices[0]);
				ind.push_back(mesh->mFaces[j].mIndices[1]);
				ind.push_back(mesh->mFaces[j].mIndices[2]);

				numofIndex += 3;
			}
			elements.push_back(new Element(ver, ind, materials[mesh->mMaterialIndex], min, max));
			ver.clear();
			ind.clear();
		}

		return true;
	}


}