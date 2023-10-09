#include "pch.h"
#include "FWork.h"

namespace BJFrameWork {

	
	FWork::FWork() :
		wnd(nullptr),
		render(nullptr),
		init(false)
	{
	}

	FWork::~FWork()
	{
	}

	void FWork::Run()
	{
		if (init)
			while (m_frame());
	}

	bool FWork::Init(const wchar_t* tittleName, const wchar_t* windowName, int width, int height, bool FullScreen)
	{
		SetWindowWidth(width);
		SetWindowHeight(height);
		SetFullScreenState(FullScreen);

		wnd = new Window(tittleName, windowName, width, height, FullScreen);

		if (!wnd)
		{
			return false;
		}
		Log::Get()->Debug("wnd was created");

		if (!Input::Get()->InitDirectInput(wnd->GetHWND()))
		{
			return false;
		};
		Log::Get()->Debug("input was created");

		render->SetHWND(wnd->GetHWND());
		if (!render->Init())
		{
			return false;
		}
		Log::Get()->Debug("render was created");

		init = true;

		

		return true;
	}

	bool FWork::m_frame()
	{
		SetIsResizedState(false);
		wnd->MainLoop();

		if (!Window::GetRunState())
			return false;


		if (!render->DrawWnd())
			return false;


		return true;
	}

	void FWork::Close()
	{
		init = false;
		CLOSE(render);
		Input::Get()->Close();
		CLOSE(wnd);
		Log::Get()->~Log();
	}


}