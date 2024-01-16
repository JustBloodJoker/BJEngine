#include "UnpackMananger.h"

namespace BJEngine
{

	UnpackMananger* UnpackMananger::instance = nullptr;
	std::string UnpackMananger::path = "";
	bool UnpackMananger::opening = false;

	UnpackMananger::UnpackMananger()
	{
		
	}

	UnpackMananger::~UnpackMananger()
	{

	}

	void UnpackMananger::Close()
	{
		delete UnpackMananger::instance;
	}

	std::vector<LightType> UnpackMananger::GetLights()
	{
		file.open(std::string(path + std::string(LIGHTNAME)));
		
		std::vector<LightType> data;
		int index = 0;

		if (file.is_open())
		{
			while (!file.eof())
			{
				data.push_back({});
				file >> data[index].pos.x >> data[index].pos.y >> data[index].pos.z >> data[index].pos.w >>
					data[index].dir.x >> data[index].dir.y >> data[index].dir.z >> data[index].dir.w >>
					data[index].color.x >> data[index].color.y >> data[index].color.z >> data[index].color.w >>
					data[index].att.x >> data[index].att.y >> data[index].att.z >>
					data[index].angle >> data[index].lightType >> data[index].enabled;
				file.get();
				index++;
			}
			data.pop_back();
			file.close();
		}
		return data;
	}

	std::vector<CameraType> UnpackMananger::GetCamera()
	{
		file.open(std::string(path + std::string(CAMERANAME)));

		std::vector<CameraType> data;
		dx::XMFLOAT4 at, eye, up;
		int index = 0;

		if (file.is_open())
		{
			while (!file.eof())
			{
				data.push_back({});
				
				file >> data[index].camPitch >> data[index].camYaw >> data[index].FoV >> data[index].moveSpeed >> data[index].sensitivity >>
					at.x >> at.y >> at.z >> at.w >>
					eye.x >> eye.y >> eye.z >> eye.w >>
					up.x >> up.y >> up.z >> up.w	;
				
				data[index].at = dx::XMVectorSet(at.x, at.y, at.z, at.w);
				data[index].up = dx::XMVectorSet(up.x, up.y, up.z, up.w);
				data[index].eye = dx::XMVectorSet(eye.x, eye.y, eye.z, eye.w);
				
				file.get();
				index++;
			}
			data.pop_back();
			file.close();
		}

		return data;
	}

	std::vector<ObjectType> UnpackMananger::GetObject()
	{
		file.open(std::string(path + std::string(OBJECTNAME)));

		std::vector<ObjectType> data;
		int index = 0;

		if (file.is_open())
		{
			while (!file.eof())
			{
				data.push_back({});

				file >> data[index].path >> data[index].prepath >> data[index].script;

				if (data[index].prepath == NONE)
					data[index].prepath = "";

				if (data[index].script == NONE)
					data[index].script = "";


				file.get();
				index++;
			}
			file.close();
		}

		return data;
	}

	std::vector<SoundType> UnpackMananger::GetSound()
	{
		return std::vector<SoundType>();
	}

	void UnpackMananger::Init(char* path)
	{
		if (!UnpackMananger::instance)
		{
			UnpackMananger::instance = new UnpackMananger();
		}

		UnpackMananger::path = std::string(path);
		PackMananger::Get()->SetPath(UnpackMananger::path);
		UnpackMananger::opening = true;
	}

	void UnpackMananger::Reset()
	{
		UnpackMananger::path = "";
		UnpackMananger::opening = false;
	}

	bool UnpackMananger::GetOpeningStatus()
	{
		return UnpackMananger::opening;
	}

}