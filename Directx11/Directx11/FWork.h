#pragma once
#include "pch.h"
#include "DirectxFramework.h"


namespace BJFrameWork {


	using namespace BJEngine;
	using namespace BJEUtils;

	class FWork
	{
	public:
		FWork();
		~FWork();

		bool Init(const wchar_t* tittleName, const wchar_t* windowName, int width, int height, bool FullScreen);
		void Run();
		void Close();

		void SetRender(Render* render) { this->render = render; }

	protected:
		bool m_frame();

		Input* input;
		Window* wnd;
		Render* render;

		bool init;
	};


}