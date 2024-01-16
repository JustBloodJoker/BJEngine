#pragma once 
#include "pch.h"

namespace BJEngine {


	class Window
	{
	public:

		Window(const wchar_t* tittleName, const wchar_t* windowName);
		~Window();

		void MainLoop();
		bool InitWindow();

		void FullScreening();

		HWND GetHWND();

		void Close();

		static bool GetRunState() { return RunRender; }

	private:

		static LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

		static bool RunRender;

		HWND hwnd;
		WNDCLASSEX wc = {};

		int width, height;
		LPWSTR tittleName, windowName;
		bool fullScreen;

	};


}