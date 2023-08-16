#pragma once
#include "pch.h"

namespace BJEngine {


	class Input
	{
	public:
		Input();
		~Input();

		void Close();

		bool InitDirectInput(HWND hwnd);

		void StartDetectInput();
		void EndDetectInput();
		bool CheckKeyState(int key) { return keyboardState[key] & 0x80; }
		bool CheckMouseMove(int xory);

		DIMOUSESTATE GetCurrState() { return mouseCurrentState; }
		DIMOUSESTATE GetLastState() { return mouseLastState; }
	private:

		IDirectInputDevice8* keyboardInput;
		IDirectInputDevice8* mouseInput;

		BYTE keyboardState[256];
		DIMOUSESTATE mouseCurrentState;
		DIMOUSESTATE mouseLastState;
		LPDIRECTINPUT8 directInput;

	};


}