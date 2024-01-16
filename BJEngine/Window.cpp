
#include "pch.h"
#include "Window.h"


extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace BJEngine {


	bool Window::RunRender = true;

	Window::Window(const wchar_t* tittleName, const wchar_t* windowName)
	{
		this->fullScreen = BJEUtils::GetFullScreenState();
		this->width = BJEUtils::GetWindowWidth();
		this->height = BJEUtils::GetWindowHeight();
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
		FullScreening();

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
		}

		this->hwnd = CreateWindowEx(NULL,
			this->windowName,
			this->tittleName,
			WS_OVERLAPPEDWINDOW,
			CW_USEDEFAULT, CW_USEDEFAULT,
			BJEUtils::GetWindowWidth(), BJEUtils::GetWindowHeight(),
			NULL,
			NULL,
			NULL,
			NULL);

		if (!this->hwnd)
		{
			Log::Get()->Err("Create wnd error");
		}

		ShowWindow(this->hwnd, 1);
		UpdateWindow(this->hwnd);

		return true;

	}

	void Window::FullScreening()
	{
		static bool alreadyUsed = false;
		if (BJEUtils::GetFullScreenState() && !alreadyUsed)
		{

			BJEUtils::SetIsResizedState(true);
			HMONITOR hmon = MonitorFromWindow(hwnd,
				MONITOR_DEFAULTTONEAREST);
			MONITORINFO mi = { sizeof(mi) };
			GetMonitorInfo(hmon, &mi);
			BJEUtils::SetWindowWidth(mi.rcMonitor.right - mi.rcMonitor.left);
			BJEUtils::SetWindowHeight(mi.rcMonitor.bottom - mi.rcMonitor.top);
			alreadyUsed = true;
		}
		else if (!BJEUtils::GetFullScreenState() && alreadyUsed)
		{
			BJEUtils::SetIsResizedState(true);
			alreadyUsed = false;
		}
	}

	HWND BJEngine::Window::GetHWND()
	{
		return this->hwnd;
	}

	LRESULT Window::WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
	{
		if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
			return true;


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
		case WM_SIZE:
		{

			BJEUtils::SetIsResizedState(false);
			BJEUtils::SetWindowHeight(HIWORD(lParam));
			BJEUtils::SetWindowWidth(LOWORD(lParam));
			SetWindowPos(hWnd, hWnd, CW_USEDEFAULT, CW_USEDEFAULT, BJEUtils::GetWindowWidth(), BJEUtils::GetWindowHeight(), WS_OVERLAPPEDWINDOW);
			
			return 0;
		}

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