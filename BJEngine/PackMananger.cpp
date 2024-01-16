#include "PackMananger.h"

namespace BJEngine
{

	PackMananger* PackMananger::instance = nullptr;
	std::ofstream PackMananger::file;
	bool PackMananger::save = false;
	std::string PackMananger::tpath = "";

	PackMananger::PackMananger()
	{
		
	}

	PackMananger::~PackMananger()
	{

	}

	void PackMananger::Close()
	{
		if (save)
		{

			save = false;	
		}
	}

	void PackMananger::AddLight(LightType element)
	{

		file.open(std::string(tpath + std::string(LIGHTNAME)), std::ios::app);

		if (file.is_open())
		{
			file << element.pos.x << " " << element.pos.y << " " << element.pos.z << " " << element.pos.w << " " <<
				element.dir.x << " " << element.dir.y << " " << element.dir.z << " " << element.dir.w << " " <<
				element.color.x << " " << element.color.y << " " << element.color.z << " " << element.color.w << " " <<
				element.att.x << " " << element.att.y << " " << element.att.z << " " <<
				element.angle << " " <<
				element.lightType << " " <<
				element.enabled << "\n";
			file.close();
		}
	}

	void PackMananger::AddCamera(CameraType element)
	{
		file.open(std::string(tpath + std::string(CAMERANAME)), std::ios::app);
		
		if (file.is_open())
		{
			file << element.camPitch << " " << element.camYaw << " " << element.FoV << " " << element.moveSpeed << " " << element.sensitivity << " " <<
				dx::XMVectorGetX(element.at) << " " << dx::XMVectorGetY(element.at) << " " << dx::XMVectorGetZ(element.at) << " " << dx::XMVectorGetW(element.at) << " " <<
				dx::XMVectorGetX(element.eye) << " " << dx::XMVectorGetY(element.eye) << " " << dx::XMVectorGetZ(element.eye) << " " << dx::XMVectorGetW(element.eye) << " " <<
				dx::XMVectorGetX(element.up) << " " << dx::XMVectorGetY(element.up) << " " << dx::XMVectorGetZ(element.up) << " " << dx::XMVectorGetW(element.up) << "\n";
			file.close();
		}

	}

	void PackMananger::AddObject(ObjectType element)
	{
		file.open(std::string(tpath + std::string(OBJECTNAME)), std::ios::app);
		
		if (element.prepath == "")
			element.prepath = NONE;
		
		if (element.script == "")
			element.script = NONE;

		if (file.is_open())
		{
			file << element.path << " " << element.prepath << " " << element.script << "\n";
			file.close();
		}
	}

	void PackMananger::AddSound(SoundType element)
	{
		file.open(std::string(tpath + std::string(SOUNDNAME)), std::ios::app);
		
		if (file.is_open())
		{
			file << element.path << " " << element.volume << "\n";
			file.close();
		}
	}

	bool PackMananger::GetSavingStatus()
	{
		return PackMananger::save;
	}

	void PackMananger::Init(std::string path)
	{

		if (!PackMananger::instance)
		{
			PackMananger::instance = new PackMananger();
			save = false;
		}

		if (path != "")
			tpath = path;

		if (!save)
		{
			bool i = true;

			std::experimental::filesystem::create_directories(path);
			
			file.open(std::string(tpath + std::string(CAMERANAME)));
			if (!file.is_open())
			{
				Log::Get()->Err("Can't save project. (CAMERA)");
				i = false;
			}
			if (file.is_open()) file.close();

			file.open(std::string(tpath + std::string(OBJECTNAME)));
			if (!file.is_open())
			{
				Log::Get()->Err("Can't save project. (OBJECTS)");
				i = false;
			}
			if (file.is_open()) file.close();

			file.open(std::string(tpath + std::string(LIGHTNAME)));
			if (!file.is_open())
			{
				Log::Get()->Err("Can't save project. (LIGHTS)");
				i = false;
			}
			if (file.is_open()) file.close();

			file.open(std::string(tpath + std::string(SOUNDNAME)));
			if (!file.is_open())
			{
				Log::Get()->Err("Can't save project. (SOUND)");
				i = false;
			}
			if (file.is_open()) file.close();

			save = i;

		}
	}

	bool PackMananger::IsPath()
	{
		return tpath != "" ? true : false;
	}

	void PackMananger::SetPath(std::string path)
	{

		if (!PackMananger::instance)
		{
			PackMananger::instance = new PackMananger();
			save = false;
		}
		tpath = path;

	}


}