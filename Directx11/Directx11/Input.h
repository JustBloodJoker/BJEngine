#pragma once
#include "pch.h"

namespace BJEngine {


	class Input
	{
	public:
		Input();
		~Input();

		static Input* Get() { return m_instance; }

		void Close();

		bool InitDirectInput(HWND hwnd);

		void StartDetectInput();
		void EndDetectInput();
		bool CheckKeyState(int key) { return keyboardState[key] & 0x80; }

		DIMOUSESTATE GetCurrState() { return mouseCurrentState; }
		DIMOUSESTATE GetLastState() { return mouseLastState; }

	private:
		
		static Input* m_instance;

		static IDirectInputDevice8* keyboardInput;
		static IDirectInputDevice8* mouseInput;

		static BYTE keyboardState[256];
		static DIMOUSESTATE mouseCurrentState;
		static DIMOUSESTATE mouseLastState;
		static LPDIRECTINPUT8 directInput;

	};


}