#pragma once 
#include "pch.h"
#include "Textures.h"
#include "Camera.h"
#include "Light.h"
#include "Model.h"
#include "Render.h"

namespace BJEngine
{


	static class UI
	{
	public:

		static bool Init(const HWND& hwnd, std::vector<BJEngine::Camera*>& pCams, Render* render );

		static bool Begin();
		static bool End();

		static void AddLog(const char* ltext, const char* text);

		static void Close();

		static bool IsInit();

		static const size_t FocusedCamera();
		static bool SetLightMananger(LightMananger* lman);

		static std::unordered_map<BJEUtils::POST_PROCESSING, bool>& GetPostProcessingStatus()noexcept;

	private:
		
		static Render* render;

		static bool ISINIT;

		static bool ParseBooleans();

		static bool IsMenuOpen(bool& check);

		static bool HotKeys();
		static bool MenuBar();
		
		static bool InitSomeTextureForGUI();
		////////////////////////


		static bool Unpack();
		static bool Pack();
		static bool LogWindow();
		static bool ConsoleWindow();
		static bool MainWindow();
		static bool AddElementsWindow();

		///////////////////////

		

		///////////////////////
		static ImGuiIO io;
		static bool unpack;
		static bool save;
		static bool saveas;
		static bool log;
		static bool console;
		static bool addElement;
		static bool mainWnd;


		///////////////////////
		//      LOG_UI
		///////////////////////
		
		static bool autoScroll;
		static ImVector<int> LineOffsets;
		static ImGuiTextBuffer buff;

		//////////////////////
		//     ELEMENTS_UI
		//////////////////////
		static std::vector<Textures*> textures_UI;
		static bool addMenuOpen;
		static bool AddElementMenu();



		static bool addLight;
		static bool CreateLight();
		static int currLightAddItem;

		static bool addCamera;
		static bool CreateCamera();
		
		static bool addModel;
		static bool CreateModel();

		static bool addSkyBox;
		static bool CreateSkyBox();

		static bool addSound;
		static bool CreateSound();

		//////////////////////
		//     MENU_UI
		//////////////////////
		static bool* isMenuOpen;

		//////////////////////
		//		MAIN_UI
		//////////////////////
		static bool iscam;
		static bool ismodel;
		static bool islight;

		////
		////   MODEL UI
		////

		static bool Model();

		/// 
		///    CAMERA UI
		/// 	
		
		static bool Camera();
		static std::vector<BJEngine::Camera*>* camera;
		static size_t focusedCamera;
		static std::string namesCams;
		static int currItemComboCams;


		/////////////////////////

		static bool cameraMoveUI;
		static bool CameraMicroMove();

		/// 
		///    LIGHT UI
		/// 
		
		static bool Light(); 
		static LightMananger* lman;
		static size_t focusedLight;
		static std::string namesLights;
		static int currItemComboLights;
		static size_t indexLight;

		/// 
		///    FOR FUTURE
		/// 

		static bool creatingState;
		static bool gameState;
		

		///
		///     POST_PROCESSING
		///
		
		static bool isPostP;
		static bool PostProcessing();

		static std::unordered_map<BJEUtils::POST_PROCESSING, bool> postProcessingBools;

	};


}