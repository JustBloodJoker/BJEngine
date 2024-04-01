#include "FileSystem.h"

namespace BJEngine
{

	FileSave::FileSave(std::string&& path, std::string&& name,
						const std::vector<BJEStruct::LightDesc>& cbLightBuffer,
						std::vector<Materials*> materials, 
						std::vector<BaseElement*> defferedElements,
						std::vector<BaseElement*> forwardElements,
						std::vector<Drawable*> drawableElements)
		: path(std::move(path)), name(std::move(name)),cbLightBuffer(cbLightBuffer), materials(materials), 
										defferedElements(defferedElements), forwardElements(forwardElements), drawableElements(drawableElements),
								file(nullptr)
	{
	}

	bool FileSave::Execute()
	{

		std::filesystem::create_directory(std::filesystem::path(path.c_str()));

		file = new std::ofstream(std::fopen(std::string(path + "/" + name).c_str(), "w"));

		if (file->is_open())
		{
			(*file) << PROJECT_FILE_START << " NAME: " << name << std::endl;

			auto curT = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());
			(*file) << "TIME: " << std::ctime(&curT) << std::endl;;
			(*file) << PROJECT_FILE_VERSION << ' ' << FRAMEWORKVERSION << std::endl;

			(*file) << std::endl;
			(*file) << PROJECT_MATERIALS_START << std::endl;

			for (const auto& el : materials)
			{
				BJEStruct::MaterialDesc dsc = el->GetMaterialDesc();
				(*file) << PROJECT_MATERIAL_NAME						 << ' ' << el->GetName() << std::endl;
				(*file) << PROJECT_MATERIAL_DIFFUSE          << ' ' << dsc.diffuse.x  << ' ' << dsc.diffuse.y  << ' ' << dsc.diffuse.z  << std::endl;
				(*file) << PROJECT_MATERIAL_AMBIENT          << ' ' << dsc.ambient.x  << ' ' << dsc.ambient.y  << ' ' << dsc.ambient.z  << std::endl;
				(*file) << PROJECT_MATERIAL_EMISSIVE         << ' ' << dsc.emissive.x << ' ' << dsc.emissive.y << ' ' << dsc.emissive.z << std::endl;
				(*file) << PROJECT_MATERIAL_SPECULAR		 << ' ' << dsc.specular.x << ' ' << dsc.specular.y << ' ' << dsc.specular.z << std::endl;
				(*file) << PROJECT_MATERIAL_SPECULAR_POWER   << ' ' << dsc.specularPower     << std::endl;
				(*file) << PROJECT_MATERIAL_OPACITY			 << ' ' << dsc.diffuse.w         << std::endl;

				std::string tmp;
				
				if ("" != (tmp = el->GetTexturePath(HAS_TEXTURE)))
				{
					(*file) << PROJECT_MATERIAL_TEXTURE_DIFFUSE << ' ' << tmp << std::endl;
				}
				if ("" != (tmp = el->GetTexturePath(HAS_NORMAL_TEXTURE)))
				{
					(*file) << PROJECT_MATERIAL_TEXTURE_NORMAL << ' ' << tmp << std::endl;
				}
				if ("" != (tmp = el->GetTexturePath(HAS_ROUGHNESS_TEXTURE)))
				{
					(*file) << PROJECT_MATERIAL_TEXTURE_ROUGHNESS << ' ' << tmp << std::endl;
				}
				if ("" != (tmp = el->GetTexturePath(HAS_SPECULAR_TEXTURE)))
				{
					(*file) << PROJECT_MATERIAL_TEXTURE_SPECULAR << ' ' << tmp << std::endl;
				}
				if ("" != (tmp = el->GetTexturePath(HAS_EMISSION_TEXTURE)))
				{
					(*file) << PROJECT_MATERIAL_TEXTURE_EMISSION << ' ' << tmp << std::endl;
				}
				(*file) << PROJECT_MATERIAL_END << std::endl;
			}

			(*file) << std::endl;

			(*file) << PROJECT_SEPARATOR << std::endl;

			(*file) << std::endl;

			(*file) << PROJECT_MODELS_DEFFERED_RENDER << std::endl;
			
			for (const auto& el : defferedElements)
			{
				auto ptr = dynamic_cast<Element*>(el);
				if (ptr == nullptr)
					continue;

				std::vector<BJEStruct::ModelVertex> vertices = ptr->GetVertices();
				std::vector<WORD> indices					 = ptr->GetIndices();

				(*file) << PROJECT_MODEL_START << std::endl;
				
				(*file) << PROJECT_MODEL_MATERIAL << ' ' << ptr->GetMaterialName() << std::endl;
				
				(*file) << PROJECT_MODEL_VERTICES << std::endl;

				for (const auto& el : vertices)
				{
					(*file) << el.pos.x << ' ' << el.pos.y << ' ' << el.pos.z << ' '
							<< el.texCoord.x << ' ' << el.texCoord.y << ' '
							<< el.normal.x << ' ' << el.normal.y << ' ' << el.normal.z << ' '
							<< el.tangent.x << ' ' << el.tangent.y << ' ' << el.tangent.z << ' '
							<< el.bitangent.x << ' ' << el.bitangent.y << ' ' << el.bitangent.z << ' '
																										<< std::endl;
				}
				(*file) << PROJECT_SEPARATOR << std::endl;


				(*file) << PROJECT_MODEL_INDICES << std::endl;

				
				for (auto iter = indices.begin(); iter != indices.end(); )
				{
					(*file) << (*iter++) << ' ';
					(*file) << (*iter++) << ' ';
					(*file) << (*iter++) << std::endl;
				}
				(*file) << PROJECT_SEPARATOR << std::endl;

				(*file) << PROJECT_MODEL_END << std::endl;
			}

			(*file) << std::endl;
			(*file) << PROJECT_SEPARATOR << std::endl;
			(*file) << std::endl;

			(*file) << PROJECT_MODELS_FORWARD_RENDER << std::endl;
			
			for (const auto& el : forwardElements)
			{
				auto ptr = dynamic_cast<ElementSkyBox*>(el);
				if (ptr == nullptr)
					continue;
				
				std::vector<BJEStruct::VertexPosOnly> vertices = ptr->GetVertices();
				std::vector<WORD> indices = ptr->GetIndices();

				(*file) << PROJECT_MODEL_START << std::endl;

				(*file) << PROJECT_SKYBOX_TEXTURE << ' ' << ptr->GetTextureName() << std::endl;

				(*file) << PROJECT_MODEL_VERTICES << std::endl;
				for (const auto& el : vertices)
				{
					(*file) << el.pos.x << ' ' << el.pos.y << ' ' << el.pos.z << ' '
						<< std::endl;
				}
				(*file) << PROJECT_SEPARATOR << std::endl;


				(*file) << PROJECT_MODEL_INDICES << std::endl;
				for (auto iter = indices.begin(); iter != indices.end(); )
				{
					(*file) << (*iter++) << ' ';
					(*file) << (*iter++) << ' ';
					(*file) << (*iter++) << std::endl;
				}
				(*file) << PROJECT_SEPARATOR << std::endl;

				(*file) << PROJECT_MODEL_END << std::endl;
			}
			
			(*file) << std::endl;
			(*file) << PROJECT_SEPARATOR << std::endl;
			(*file) << std::endl;

			(*file) << PROJECT_LIGHTS << std::endl;

			(*file) << PROJECT_LIGHTS_COUNT << ' ' << cbLightBuffer.size() << std::endl;
			(*file) << PROJECT_LIGHTS_PARAMS_START << std::endl;
			for (size_t ind = 0; ind < cbLightBuffer.size(); ind++)
			{
				(*file) << cbLightBuffer[ind].pos.x << ' ' << cbLightBuffer[ind].pos.y << ' ' << cbLightBuffer[ind].pos.z << ' '
						<< cbLightBuffer[ind].lightType << ' '
						<< cbLightBuffer[ind].shadowEnabled << ' '
						<< cbLightBuffer[ind].enabled << ' '
						<< cbLightBuffer[ind].color.x << ' ' << cbLightBuffer[ind].color.y << ' ' << cbLightBuffer[ind].color.z << ' '
						<< cbLightBuffer[ind].att.x << ' ' << cbLightBuffer[ind].att.y << ' ' << cbLightBuffer[ind].att.z << ' '
						<< cbLightBuffer[ind].angle << ' '
						<< cbLightBuffer[ind].dir.x << ' ' << cbLightBuffer[ind].dir.y << ' ' << cbLightBuffer[ind].dir.z << ' '
						<< std::endl;
			}
			(*file) << PROJECT_LIGHTS_PARAMS_END << std::endl;


			(*file) << PROJECT_SEPARATOR << std::endl;
			(*file) << std::endl;
			(*file) << PROJECT_FILE_END << std::endl;
			file->close();
			DELETE(file);
		}
		else
		{
			Log::Get()->Err("CAN'T OPEN FILE TO SAVE THE PROJECT!");
			return false;
		}
		return true;
	}

	void FileOpen::InitMaterial(Materials* mater)
	{
		char buff[400];
		std::istringstream ss;
		std::string chk;
		dx::XMFLOAT4 params = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

		while (chk != PROJECT_MATERIAL_END)
		{
			file->getline(buff, 400, '\n');
			ss.clear();
			ss.str(buff);

			ss >> chk;
			if (chk == PROJECT_MATERIAL_DIFFUSE)
			{
				ss >> params.x >> params.y >> params.z;
				mater->SetParam(DIFFUSE, params);
			}
			else if (chk == PROJECT_MATERIAL_AMBIENT)
			{
				ss >> params.x >> params.y >> params.z;
				mater->SetParam(AMBIENT, params);
			}
			else if (chk == PROJECT_MATERIAL_EMISSIVE)
			{
				ss >> params.x >> params.y >> params.z;
				mater->SetParam(EMISSIVE, params);
			}
			else if (chk == PROJECT_MATERIAL_SPECULAR)
			{
				ss >> params.x >> params.y >> params.z;
				mater->SetParam(SPECULAR, params);
			}
			else if (chk == PROJECT_MATERIAL_OPACITY)
			{
				ss >> params.w;
				mater->SetParam(OPACITY, params.w);
			}
			else if (chk == PROJECT_MATERIAL_SPECULAR_POWER)
			{
				ss >> params.w;
				mater->SetParam(SPECULAR_POWER, params.w);
			}
			else if (chk == PROJECT_MATERIAL_TEXTURE_DIFFUSE)
			{
				ss >> chk;
				mater->SetTexture(HAS_TEXTURE, std::wstring(chk.begin(), chk.end()));
			}
			else if (chk == PROJECT_MATERIAL_TEXTURE_NORMAL)
			{
				ss >> chk;
				mater->SetTexture(HAS_NORMAL_TEXTURE, std::wstring(chk.begin(), chk.end()));
			}
			else if (chk == PROJECT_MATERIAL_TEXTURE_ROUGHNESS)
			{
				ss >> chk;
				mater->SetTexture(HAS_ROUGHNESS_TEXTURE, std::wstring(chk.begin(), chk.end()));
			}
			else if (chk == PROJECT_MATERIAL_TEXTURE_EMISSION)
			{
				ss >> chk;
				mater->SetTexture(HAS_EMISSION_TEXTURE, std::wstring(chk.begin(), chk.end()));
			}
			else if (chk == PROJECT_MATERIAL_TEXTURE_SPECULAR)
			{
				ss >> chk;
				mater->SetTexture(HAS_SPECULAR_TEXTURE, std::wstring(chk.begin(), chk.end()));
			}
			else
			{

			}
		}
	}

	void FileOpen::InitModels(std::vector<BaseElement*>* elVec)
	{
		char buff[400];
		std::istringstream ss;
		std::string chk;
		BJEStruct::ModelVertex el;
		std::vector<BJEStruct::ModelVertex> vertices;
		WORD tmpIndex;
		std::vector<WORD> indices;
		Materials* tmpPtr = nullptr;
		dx::XMFLOAT3 minExtent = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
		dx::XMFLOAT3 maxExtent = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);

		while (chk != PROJECT_SEPARATOR)
		{
			file->getline(buff, 400, '\n');
			ss.clear();
			ss.str(buff);
			ss >> chk;
			if (chk == PROJECT_MODEL_START)
			{
				while (chk != PROJECT_MODEL_END)
				{
					file->getline(buff, 400, '\n');
					ss.clear();
					ss.str(buff);
					ss >> chk;
					if (chk == PROJECT_MODEL_MATERIAL)
					{
						ss >> chk;
						tmpPtr = mater[chk];
					}
					else if (chk == PROJECT_MODEL_VERTICES)
					{
						while (chk != PROJECT_SEPARATOR)
						{
							
							file->getline(buff, 400, '\n');
							ss.clear();
							ss.str(buff);
							
							ss >> chk;
							ss.clear();
							ss.seekg(0);

							if (chk == PROJECT_SEPARATOR)
								continue;

							ss >> el.pos.x >> el.pos.y >> el.pos.z
								>> el.texCoord.x >> el.texCoord.y
								>> el.normal.x >> el.normal.y >> el.normal.z
								>> el.tangent.x >> el.tangent.y >> el.tangent.z
								>> el.bitangent.x >> el.bitangent.y >> el.bitangent.z;

							vertices.push_back(el);
							if (vertices.size() > 1)
							{
								minExtent.x = std::min(minExtent.x, el.pos.x);
								minExtent.y = std::min(minExtent.y, el.pos.y);
								minExtent.z = std::min(minExtent.z, el.pos.z);

								maxExtent.x = std::max(maxExtent.x, el.pos.x);
								maxExtent.y = std::max(maxExtent.y, el.pos.y);
								maxExtent.z = std::max(maxExtent.z, el.pos.z);
							}
							else
							{
								minExtent.x = el.pos.x;
								minExtent.y = el.pos.y;
								minExtent.z = el.pos.z;
								maxExtent.x = el.pos.x;
								maxExtent.y = el.pos.y;
								maxExtent.z = el.pos.z;
							}
						}
						

					}
					else if (chk == PROJECT_MODEL_INDICES)
					{
						while (chk != PROJECT_SEPARATOR)
						{
							file->getline(buff, 400, '\n');
							ss.clear();
							ss.str(buff);

							ss >> chk;
							ss.clear();
							ss.seekg(0);
							
							if (chk == PROJECT_SEPARATOR)
								continue;

							ss >> tmpIndex;
							indices.push_back(tmpIndex);
							ss >> tmpIndex;
							indices.push_back(tmpIndex);
							ss >> tmpIndex;
							indices.push_back(tmpIndex);
						}
					}
				}
				(*elVec).push_back(new Element(std::move(vertices), std::move(indices), tmpPtr, dx::XMVectorSet(minExtent.x, minExtent.y, minExtent.z, 1.0f), dx::XMVectorSet(maxExtent.x, maxExtent.y, maxExtent.z, 1.0f)));
				(*drawableElements).push_back(*(elVec->end() - 1));
			}
		}
	}

	void FileOpen::InitSkyBox(std::vector<BaseElement*>* elVec)
	{
		char buff[400];
		std::istringstream ss;
		std::string chk;
		BJEStruct::VertexPosOnly el;
		std::vector<BJEStruct::VertexPosOnly> vertices;
		WORD tmpIndex;
		std::vector<WORD> indices;
		Textures* tmpPtr = nullptr;

		while (chk != PROJECT_SEPARATOR)
		{
			file->getline(buff, 400, '\n');
			ss.clear();
			ss.str(buff);
			ss >> chk;
			if (chk == PROJECT_MODEL_START)
			{
				while (chk != PROJECT_MODEL_END)
				{
					file->getline(buff, 400, '\n');
					ss.clear();
					ss.str(buff);
					ss >> chk;
					if (chk == PROJECT_SKYBOX_TEXTURE)
					{
						ss >> chk;
						tmpPtr = new Textures(std::wstring(chk.begin(), chk.end()));
						tmpPtr->InitCubeMap();
					}
					else if (chk == PROJECT_MODEL_VERTICES)
					{
						while (chk != PROJECT_SEPARATOR)
						{
							file->getline(buff, 400, '\n');
							ss.clear();
							ss.str(buff);

							ss >> chk;
							ss.clear();
							ss.seekg(0);

							if (chk == PROJECT_SEPARATOR)
								continue;

							ss >> el.pos.x >> el.pos.y >> el.pos.z;

							vertices.push_back(el);
						}
					}
					else if (chk == PROJECT_MODEL_INDICES)
					{
						while (chk != PROJECT_SEPARATOR)
						{
							file->getline(buff, 400, '\n');
							ss.clear();
							ss.str(buff);

							ss >> chk;
							ss.clear();
							ss.seekg(0);

							if (chk == PROJECT_SEPARATOR)
								continue;

							ss >> tmpIndex;
							indices.push_back(tmpIndex);
							ss >> tmpIndex;
							indices.push_back(tmpIndex);
							ss >> tmpIndex;
							indices.push_back(tmpIndex);
						}
					}
				}
				(*elVec).push_back(new ElementSkyBox(tmpPtr));
			}
		}


	}

	void FileOpen::InitLightCBData(std::vector<BJEStruct::LightDesc>* cbLightBuffer)
	{
		char buff[400];
		std::istringstream ss;
		std::string chk;
		size_t index = 0;

		while (chk != PROJECT_SEPARATOR)
		{
			file->getline(buff, 400, '\n');
			ss.clear();
			ss.str(buff);
			ss >> chk;

			if (chk == PROJECT_LIGHTS_COUNT)
			{
				
			}
			else if (chk == PROJECT_LIGHTS_PARAMS_START)
			{
				while (chk != PROJECT_LIGHTS_PARAMS_END)
				{
					file->getline(buff, 400, '\n');
					ss.clear();
					ss.str(buff);
					ss >> chk;
					ss.clear();
					ss.seekg(0);
					if (chk == PROJECT_LIGHTS_PARAMS_END)
						continue;

					
					BJEStruct::LightDesc ds;
					ss >> ds.pos.x >> ds.pos.y >> ds.pos.z
						>> ds.lightType
						>> ds.shadowEnabled
						>> ds.enabled
						>> ds.color.x >> ds.color.y >> ds.color.z
						>> ds.att.x >> ds.att.y >> ds.att.z
						>> ds.angle
						>> ds.dir.x >> ds.dir.y >> ds.dir.z;

					cbLightBuffer->push_back(ds);
				}
			}
		}
	}

	FileOpen::FileOpen(std::string&& path, std::vector<BaseElement*>* defferedElements, std::vector<BaseElement*>* forwardElements, std::vector<Drawable*>* drawableElements, std::vector<BJEStruct::LightDesc>* cbLightBuffer)
		: path(std::move(path)), defferedElements(defferedElements), forwardElements(forwardElements), cbLightBuffer(cbLightBuffer), drawableElements(drawableElements)
	{
		
	}

	bool FileOpen::Execute()
	{
		file = new std::ifstream(std::fopen(path.c_str(), "r"));

		char buff[400];
		std::istringstream ss;
		std::string chk;

		if (file->is_open())
		{
			while (!file->eof())
			{
				file->getline(buff, 400, '\n');
				chk.clear();
				ss.clear();
				ss.str(buff);

				ss >> chk;

				if		(chk == PROJECT_FILE_VERSION)
				{
					
				}
				else if (chk == PROJECT_MATERIAL_NAME)
				{
					ss >> chk;
					mater[chk] = new Materials();
					InitMaterial(mater[chk]);

				}
				else if (chk == PROJECT_MODELS_DEFFERED_RENDER)
				{
					InitModels(defferedElements);
				}
				else if (chk == PROJECT_MODELS_FORWARD_RENDER)
				{
					InitSkyBox(forwardElements);
				}
				else if (chk == PROJECT_LIGHTS)
				{
					InitLightCBData(cbLightBuffer);
				}
			}

			file->close();
		}
		else
		{
			Log::Get()->Err("CAN'T OPEN FILE TO OPEN THE PROJECT!");
			return false;
		}


		return true;
	}

}