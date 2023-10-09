#pragma once

namespace BJEUtils {

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

#define FRAMEWORKVERSION 1

#define LOGNAME "log.txt"

	std::wstring aiStringToWString(aiString aiStr);


#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024
}