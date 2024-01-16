#pragma once
#include "pch.h"

namespace BJEngine
{


	class PackMananger
	{
		friend class UnpackMananger;

	public:
		PackMananger();
		~PackMananger();
		void Close();
		static PackMananger* Get() { return instance; }

		void AddLight(LightType element);
		void AddCamera(CameraType element);
		void AddObject(ObjectType element);
		void AddSound(SoundType element);
	
		bool GetSavingStatus();
		void Init(std::string path = "");
		
		bool IsPath();
	private:

		void SetPath(std::string path);
		static PackMananger* instance;

		static std::ofstream file;

		static bool save;
		static std::string tpath;
	};


}