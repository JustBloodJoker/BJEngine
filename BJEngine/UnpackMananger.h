#pragma once
#include "pch.h"
#undef GetObject

namespace BJEngine
{


	class UnpackMananger
	{
	public:

		UnpackMananger();
		~UnpackMananger();

		void Close();
		static UnpackMananger* Get() { return instance; }

		std::vector<LightType> GetLights();
		std::vector<CameraType> GetCamera();
		std::vector<ObjectType> GetObject();
		std::vector<SoundType> GetSound();

		void Init(char* path);
		void Reset();
		bool GetOpeningStatus();

	private:

		static UnpackMananger* instance;
		std::ifstream file;
		static std::string path;
		static bool opening;

	};


}