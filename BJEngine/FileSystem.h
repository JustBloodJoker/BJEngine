#pragma once
#include "pch.h"
#include "Materials.h"
#include "Element.h"
#include "FileSystemHelper.h"

namespace BJEngine
{



	class FileSystem
	{
	public:

		virtual bool Execute() = 0;

	};

	class FileSave
		: public FileSystem
	{
		std::vector<Drawable*> drawableElements;
		std::vector<BaseElement*> defferedElements;
		std::vector<BaseElement*> forwardElements;
		std::vector<Materials*> materials;
		std::vector<BJEStruct::LightDesc> cbLightBuffer;
		std::ofstream* file;
		std::string path;
		std::string name;

	public:

		FileSave(std::string&& path,std::string&& name,const std::vector<BJEStruct::LightDesc>& cbLightBuffer, std::vector<Materials*> materials, std::vector<BaseElement*> defferedElements, std::vector<BaseElement*> forwardElements, std::vector<Drawable*> drawableElements);

		bool Execute() override;

	};

	class FileOpen
		: public FileSystem
	{
		std::vector<Drawable*>* drawableElements;
		std::vector<BaseElement*>* defferedElements;
		std::vector<BaseElement*>* forwardElements;
		std::vector<BJEStruct::LightDesc>* cbLightBuffer;
		std::ifstream* file;
		std::string path;
		std::unordered_map<std::string, Materials*> mater;
		
		void InitMaterial(Materials* mater);
		void InitModels(std::vector<BaseElement*>*);
		void InitSkyBox(std::vector<BaseElement*>*);
		void InitLightCBData(std::vector<BJEStruct::LightDesc>* cbLightBuffer);

	public:

		FileOpen(std::string&& path, std::vector<BaseElement*>* defferedElements, std::vector<BaseElement*>* forwardElements, std::vector<Drawable*>* drawableElements, std::vector<BJEStruct::LightDesc>* cbLightBuffer);

		bool Execute() override;

	};

}