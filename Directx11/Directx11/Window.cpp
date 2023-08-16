#include "pch.h"
#include "Window.h"

namespace BJEngine {


	bool Window::RunRender = true;

	Window::Window(const wchar_t* tittleName, const wchar_t* windowName, int width, int height, bool FullScreen)
	{
		this->fullScreen = FullScreen;
		this->width = width;
		this->height = height;
		this->tittleName = (LPWSTR)tittleName;
		this->windowName = (LPWSTR)windowName;

		if (!this->InitWindow())
		{
			MessageBox(0, L"Window Initialization - Failed",
				L"Error", MB_OK);
		}

		this->MainLoop();
	}

	Window::~Window() = default;

	void Window::MainLoop()
	{
		MSG msg;
		ZeroMemory(&msg, sizeof(MSG));

		while (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	bool Window::InitWindow()
	{

		FullScreening();

		this->wc.cbSize = sizeof(WNDCLASSEX);
		this->wc.style = CS_HREDRAW | CS_VREDRAW;
		this->wc.lpfnWndProc = Window::WndProc;
		this->wc.cbClsExtra = NULL;
		this->wc.cbWndExtra = NULL;
		this->wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
		this->wc.hCursor = LoadCursor(NULL, IDC_ARROW);
		this->wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
		this->wc.lpszMenuName = NULL;
		this->wc.lpszClassName = this->windowName;
		this->wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

		if (!RegisterClassEx(&this->wc))
		{
			Log::Get()->Err("Register wnd class error");
			exit(Log::WNDCLASSERRORS);
		}

		this->hwnd = CreateWindowEx(NULL,
			this->windowName,
			this->tittleName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			this->width, this->height,
			NULL,
			NULL,
			NULL,
			NULL);

		if (!this->hwnd)
		{
			Log::Get()->Err("Create wnd error");
			exit(Log::WNDCLASSERRORS);
		}

		ShowWindow(this->hwnd, 1);
		UpdateWindow(this->hwnd);

		return true;

	}

	void Window::FullScreening()
	{
		if (this->fullScreen)
		{
			HMONITOR hmon = MonitorFromWindow(hwnd,
				MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(hmon, &mi);

			this->width = mi.rcMonitor.right - mi.rcMonitor.left;
			this->height = mi.rcMonitor.bottom - mi.rcMonitor.top;
		}
	}

	HWND BJEngine::Window::GetHWND()
	{
		return this->hwnd;
	}

	LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		switch (msg)
		{
			break;
		case WM_KEYDOWN:
			if (wParam == VK_ESCAPE) {
				if (MessageBox(0, L"Are you sure you want to exit?",
					L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
				{

					RunRender = false;
					DestroyWindow(hWnd);
					
				}
			}
			return 0;

		case WM_DESTROY:
			RunRender = false;
			PostQuitMessage(0);
			return 0;
		}
		return DefWindowProc(hWnd,
			msg,
			wParam,
			lParam);
	}

	void Window::Close()
	{
		if (hwnd) 
		{
			DestroyWindow(hwnd);
			Log::Get()->Debug("Destroing wnd");
		}
		hwnd = nullptr;
		
	}


}