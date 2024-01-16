#include "Model.h"
#include "UI.h"

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
			{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
			{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 20, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 32, D3D11_INPUT_PER_VERTEX_DATA, 0},
			{ "BITANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 44, D3D11_INPUT_PER_VERTEX_DATA, 0}
		};
		if (shader == nullptr)
		{
			shader = new BJEngine::Shader(L"shaders\\ObjectShaderPS.hlsl", L"shaders\\ObjectShaderVS.hlsl", "VS", "PS");
		}

		shader->SetInputLayout(layout, ARRAYSIZE(layout));
		shader->Init(pd3dDevice);

		pConstantBuffer = Helper::InitConstantBuffer<Object::ConstantBuffer>(pd3dDevice);

		for (auto& el : elements)
		{
			el->Init(pd3dDevice, pConstantBuffer);
		}

		
		rotation = dx::XMMatrixRotationY(0.0f);
		scale = dx::XMMatrixScaling(1.0f, 1.0f, 1.0f);
		pos = dx::XMMatrixTranslation(0.0f, 0.0f, 0.0f);

		world = rotation * scale * pos;
		outLine = dx::XMMatrixScaling(1.05f, 1.05f, 1.05f) + rotation * scale * pos ;

		Log::Get()->Debug("Model was inited");
		isInited = true;
		
		if(!script)
			script = new Script();

		UI::AddModelToList(this);

		return true;
	}

	void Model::Draw()
	{
		UINT stride = sizeof(BJEStruct::ModelVertex);
		UINT offset = 0;

		ImGui::Begin(filename.c_str());

		ImGui::Text("Set script");
		ImGui::InputText(" ", buffer, 100);
		if (ImGui::Button("Set"))
		{
			script->ParseScript(buffer);
		};
		
		if(!isSimulate)
			isSimulate = ImGui::Button("Start Script");

		if (isSimulate)
		{
			if (!returned)
			{
				bworld = world;
				returned = true;
			}
			world *= script->DrawScript();
			
			isSimulate = !ImGui::Button("Stop Script");
		}
		else if(returned)
		{
			returned = false;
			world = bworld;
			script->ResetParameters();
		}

		ImGui::End();

		for (auto& el : elements)
		{
			objectBox.CreateFromPoints(objectBox, dx::XMVector3Transform(el->GetMaxLocal(), world),
				dx::XMVector3Transform(el->GetMinLocal(), world)
			);
			
			if (cam->GetFrustum().Intersects(objectBox) || cam->GetFrustum().Contains(objectBox))
			{
				pImmediateContext->IASetInputLayout(shader->GetInputLayout());
				pImmediateContext->VSSetShader(shader->GetVertexShader(), NULL, 0);
				pImmediateContext->PSSetShader(shader->GetPixelShader(), NULL, 0);
				el->Draw(pImmediateContext, &stride, &offset, world, outLine, cam->GetViewMatrix(), cam->GetProjectionMatrix(), lView, lProjection);
			}
		}

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

	void Model::MinDraw()
	{
		UINT stride = sizeof(BJEStruct::ModelVertex);
		UINT offset = 0;

		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		for (auto& el : elements)
		{
			el->MinDraw(pImmediateContext, &stride, &offset);
		}
	}

	void Model::SetScript(std::string data)
	{
		script = new Script();
		script->ParseScript(_strdup(data.c_str()));
	}

	bool Model::LoadModel()
	{
		scene = importer.ReadFile(filename,
			aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_GenNormals | aiProcess_CalcTangentSpace);
		if (!scene)
			return false;

		for (int i = 0; i < scene->mNumMaterials; i++)
		{
			materials.push_back(new Materials(pd3dDevice));

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
			
			scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_EMISSIVE, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_EMISSION_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname), pd3dDevice);
			}
			pathname.Clear();

			scene->mMaterials[i]->GetTexture(aiTextureType::aiTextureType_SPECULAR, 0, &pathname);
			if (pathname.length != 0)
			{
				materials[i]->SetTexture(HAS_SPECULAR_TEXTURE, texturePrefixPath + BJEUtils::aiStringToWString(pathname), pd3dDevice);
			}
			pathname.Clear();

		}

		dx::XMFLOAT3 minExtent;
		dx::XMFLOAT3 maxExtent;

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
			}

			elements.push_back(new Element(ver, ind, materials[mesh->mMaterialIndex], min, max, pd3dDevice));
			ver.clear();
			ind.clear();
		}
		return true;
	}


	bool Model::InitAnimation()
	{





		return true;
	}



}