#pragma once 
#include "pch.h"
#include "Light.h"
#include "Sound.h"
#include "MainCamera.h"
#include "Object.h"
#include "BackGround.h"
#include "RenderTarget.h"

namespace BJEngine {


	class Render
	{
	public:

		Render()
		{
			driverType = D3D_DRIVER_TYPE_NULL;
			featureLevel = D3D_FEATURE_LEVEL_11_0;
			pSwapChain = nullptr;
			mainRTV = nullptr;
		}
		~Render() {  };

		bool CreateDevice();
		void BeginFrame();
		void EndFrame();

		bool Init();
		bool DrawWnd();
		Object* InitObjs(Object* object);
		void AddSkyBox(std::string texturePath);
		void ResizeWindow();

		void Close();
		void SetHWND(HWND hwnd) { this->hwnd = hwnd; }

		void* operator new(size_t i)
		{
			return _aligned_malloc(i, 16);
		}

		void operator delete(void* p)
		{
			_aligned_free(p);
		}

		void SetObjectsVector(std::vector<Object*> obj) { this->objects = obj; }

		void CreateSound(std::string path);

		void UnpackProject();

		void SetLight(LightDesc ld);

	private:

		bool DrawScene();

		std::vector<Object*> objects;
		std::vector<BJAudio::Sound*> sound;
		std::vector<Camera*> cams;

		bool islight = false;
		bool isInitlight = false;

		BackGround* skyBox = nullptr;

		HWND hwnd;

		D3D_DRIVER_TYPE driverType;
		D3D_FEATURE_LEVEL featureLevel;

		IDXGISwapChain* pSwapChain;

		RenderTarget* mainRTV;
		RenderTarget* sceneRTV;
		
		D3D11_VIEWPORT vp;
		D3D11_VIEWPORT svp;

		LightDesc ld;
	};


}
