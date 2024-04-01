#pragma once 
#include "pch.h"
#include "Light.h"
#include "Sound.h"
#include "LightMananger.h"
#include "MainCamera.h"
#include "Object.h"
#include "RenderTarget.h"
#include "FileSystem.h"

namespace BJEngine {


	class Render
	{
	protected:

		friend class UI;

	public:

		Render()
		{
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

		void SetObjectsVector(std::vector<Object*> obj) { this->objects = obj; }

		void CreateSound(std::string path);

		void SetLight(BJEStruct::LightDesc ld);
		
		//////////////////
		//FILE

		void LoadProject(std::string path);
		void SaveProject(std::string path, std::string name);

		Drawable*& GetDrawable(size_t index);
	protected:

		BJEStruct::MainSceneProcessingBuffer mainDesc;

	private:

		void ExecuteFileSystemCommand();
		std::queue<FileSystem*> fileSysExecution;

		bool DrawScene();
		
		std::vector<Drawable*> drawableOnScene;
		size_t indexStr = 0;
		std::vector<BaseElement*> defferedElements;
		std::vector<BaseElement*> forwardElements;
		std::vector<Object*> objects;
		std::vector<BJAudio::Sound*> sound;
		std::vector<Camera*> cams;

		bool islight = false;
		bool isInitlight = false;

		HWND hwnd;

		IDXGISwapChain* pSwapChain;

		std::vector<Shadow*> shadow;

		DepthStencil* dsv;

		RenderTarget* mainRTV;
		ID3D11Buffer* mainRTVBuffer;
		RenderTarget* sceneRTV;
		RenderTarget* normalsRTV;
		RenderTarget* diffuseRTV;
		RenderTarget* specularRTV;
		RenderTarget* roughnessRTV;

		RenderTarget* irradianceRTV;

		D3D11_VIEWPORT vp;
		D3D11_VIEWPORT svp;

		LightMananger* lmananger;

		std::vector<BJEStruct::LightDesc> ldtmp;

		ElementSkyBoxConvertion* elementSkyBoxConvertion;
	};


}
