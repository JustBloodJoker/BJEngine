#pragma once

namespace BJEUtils {

#define DELETE(p)		{ if(p){delete (p); (p)=nullptr;} }
#define DELETEARRAY(p)	{ if(p){delete[] (p); (p)=nullptr;} }
#define RELEASE(p)		{ if(p){(p)->Release(); (p)=nullptr;} }
#define CLOSE(p)		{ if(p){(p)->Close(); delete (p); (p)=nullptr;} }
#define LCLOSE(p)		{ if(p){delete (p); (p)=nullptr; } }
#define UNACQUIRE(p)    { if(p){(p)->Unacquire();} }

#define WIDTH 1024
#define HEIGHT 1024

	enum {
		_WINDOWED,
		_FULLSCREEN
	};


#define FRAMEWORKVERSION 1

#define LOGNAME "log.txt"

}