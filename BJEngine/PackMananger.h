#pragma once
#include "pch.h"
#include "PackageUtilites.h"

namespace BJEngine
{


	

	class PackMananger
	{
	public:
		PackMananger();
		~PackMananger();
		void Close();
		static PackMananger* Get() { return instance; }

		void AddLight(LightType element);
		void AddCamera(CameraType element);
		void AddObject(ObjectType element);
		void AddSound(SoundType element);
		
		void SetSavingStatus(bool cc);
		bool GetSavingStatus();
		void Init(char* path);
	private:
		static PackMananger* instance;

		static std::ofstream file;

		static bool save;
		static bool isInit;
		std::string tpath;
	};


}