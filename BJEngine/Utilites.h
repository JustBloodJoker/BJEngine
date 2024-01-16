#pragma once
#include "framework.h"


namespace BJEUtils 
{
#define DIFFUSE_TEXTURE_POS      0
#define NORMAL_TEXTURE_POS       1
#define ROUGHNESS_TEXTURE_POS    2
#define EMISSION_TEXTURE_POS     3
#define SPECULAR_TEXTURE_POS     4

#define DIFFUSE_SAMPLERSTATE_POS      0
#define NORMAL_SAMPLERSTATE_POS       0
#define ROUGHNESS_SAMPLERSTATE_POS    0
#define EMISSION_SAMPLERSTATE_POS     0
#define SPECULAR_SAMPLERSTATE_POS     0




#define DELETE(p)		{ if(p){delete (p); (p)=nullptr;} }
#define DELETEARRAY(p)	{ if(p){delete[] (p); (p)=nullptr;} }
#define RELEASE(p)		{ if(p){(p)->Release(); (p)=nullptr;} }
#define CLOSE(p)		{ if(p){(p)->Close(); delete (p); (p)=nullptr;} }
#define LCLOSE(p)		{ if(p){delete (p); (p)=nullptr; } }
#define UNACQUIRE(p)    { if(p){(p)->Unacquire();} }

	float GetWindowWidth();
	float GetWindowHeight();
	bool GetFullScreenState();
	bool GetIsResizedState();

	void SetFullScreenState(bool F);
	void SetWindowWidth(float W);
	void SetWindowHeight(float H);
	void SetIsResizedState(bool isRes);

	enum {
		_WINDOWED,
		_FULLSCREEN
	};

	enum {
		SPOTLIGHT,
		POINTLIGHT,
		DIRECTIONALLIGHT
	};

#define FRAMEWORKVERSION 2

#define LOGNAME "log.txt"

	std::wstring aiStringToWString(aiString aiStr);
	std::wstring charToWString(const char* cch);
	std::string ParseLightTypeToStr(int type);

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

#define MAX_LIGHT_NUM 5

	//////////////////////////////////////
	//					UI
	//////////////////////////////////////

#define UI_OPENMENU_WIDTH         340
#define UI_OPENMENU_HEIGHT        150

#define UI_LOGMENU_WIDTH          500
#define UI_LOGMENU_HEIGHT         300

#define UI_MAINMENU_HEIGHT        540
#define UI_MAINMENU_WIDTH         400

#define UI_ADDELEMENTMENU_WIDTH   800
#define UI_ADDELEMENTMENU_HEIGHT  300

#define UI_MOVECAMERAFACE_WIDTH   400
#define UI_MOVECAMERAFACE_HEIGHT  300

	//костыль тоже заебал
#define MODELS_SAMPLES_PREPATH "sampleModels/"


#define UI_ADDELEMENTMENU_LIGHTADD_COMBO "SpotLight\0PointLight\0"


#define UI_TEXTURE_LAMP_PATH L"models//textures//BLL.png"
#define UI_TEXTURE_CAMERA_PATH L"models//textures//CAMERA.png"
#define UI_TEXTURE_ADDELEMENT_PATH L"models//textures//ADD.png"
#define UI_TEXTURE_MODEL_PATH L"models//textures//MODELICO.png"
#define UI_TEXTURE_SKYBOX_PATH L"models//textures//SKYBOX.png"
#define UI_TEXTURE_SOUND_PATH L"models//textures//SOUNDICO.png"

#define UI_TEXTURES_ADDBUTTON           0
#define UI_TEXTURES_ADDLIGHTBUTTON      1
#define UI_TEXTURES_ADDCAMERABUTTON     2
#define UI_TEXTURES_ADDMODELBUTTON      3
#define UI_TEXTURES_ADDSKYBOXBUTTON     4
#define UI_TEXTURES_ADDSOUNDBUTTON      5


#define UI_TEXTURES_ADDSCRIPTBUTTON     3
#define UI_TEXTURES_ADDSPHEREBUTTON     4
#define UI_TEXTURES_ADDCONEBUTTON       5
#define UI_TEXTURES_ADDCUBEBUTTON       6 

}

