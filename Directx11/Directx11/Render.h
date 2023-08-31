#pragma once

#include "pch.h"

#include "Light.h"
#include "Input.h"
#include "Camera.h"
#include "Object.h"
#include "Sound.h"

namespace BJEngine {


	class Render
	{
	public:

		Render()
		{
			driverType = D3D_DRIVER_TYPE_NULL;
			featureLevel = D3D_FEATURE_LEVEL_11_0;
			pd3dDevice = nullptr;
			pImmediateContext = nullptr;
			pSwapChain = nullptr;
			pRenderTargetView = nullptr;
		}
		~Render() {  };

		bool CreateDevice();
		void BeginFrame();
		void EndFrame();

		bool Init();
		bool DrawWnd();
		Object* InitObjs(Object* object);


		virtual bool DrawActions();
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

		ID3D11Device* GetDevice() { return pd3dDevice; }
		ID3D11DeviceContext* GetContext(){ return pImmediateContext; }
		Camera* GetCamera() { return cam; }

		void SetObjectsVector(std::vector<Object*> obj) { this->objects = obj; }
		void SetLight(LightDesc* ld, int typeOfLight);
		Light* GetLight() { return light; }
	private:
		
		std::vector<Object*> objects;

		bool islight = false;
		bool isInitlight = false;
		BJAudio::Sound* sound;
		Camera* cam;
		
		ID3D11DepthStencilView* depthStencilView;
		ID3D11Texture2D* depthStencilBuffer;

		ID3D11Device* pd3dDevice;
		ID3D11DeviceContext* pImmediateContext;
	
		
		HWND hwnd;

		D3D_DRIVER_TYPE driverType;
		D3D_FEATURE_LEVEL featureLevel;

		IDXGISwapChain* pSwapChain;
		ID3D11RenderTargetView* pRenderTargetView;

		Light* light = nullptr;
	

	};


}
