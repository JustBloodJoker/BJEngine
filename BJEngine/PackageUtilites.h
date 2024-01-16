#pragma once
#define CAMERANAME "\\Camera.txt"
#define SOUNDNAME "\\Sound.txt"
#define OBJECTNAME "\\Objects.txt"
#define LIGHTNAME "\\Lights.txt"
#define NONE "none"

enum
{

	SOUND,
	OBJECT,
	LIGHT,
	CAMERA

};

struct SoundType
{
	std::string path;
	float volume;
};

struct ObjectType
{
	std::string path;
	std::string prepath;
	std::string script;
};

struct LightType
{
	dx::XMFLOAT4 pos;
	dx::XMFLOAT4 dir;
	dx::XMFLOAT4 color;
	dx::XMFLOAT3 att;
	float angle;
	int lightType;
	int enabled;
};

struct CameraType
{
	dx::XMVECTOR eye;
	dx::XMVECTOR at;
	dx::XMVECTOR up;
	float camYaw;
	float camPitch;
	float moveSpeed = 500.0f;
	float sensitivity = 0.001f;
	float FoV = M_PI / 2;
};