#include "pch.h"
#include "Input.h"

namespace BJEngine {


    Input::Input()
    {
        keyboardInput = nullptr;
        mouseInput = nullptr;
    }

    Input::~Input()
    {

    }

    void Input::Close()
    {
        UNACQUIRE(keyboardInput);
        UNACQUIRE(mouseInput);
        RELEASE(directInput);
        Log::Get()->Debug("Close input");
    }

    bool Input::InitDirectInput(HWND hwnd)
    {
        HRESULT hr = DirectInput8Create(GetModuleHandle(NULL), DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&directInput, NULL);

        if (FAILED(hr))
        {
            Log::Get()->Err("DirectInput8Create error");
            exit(Log::DIERROR);
        }

        hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboardInput, NULL);
        if (FAILED(hr)) 
        {
            Log::Get()->Err("CreateDevice DI error");
            exit(Log::DIERROR);
        }

        hr = directInput->CreateDevice(GUID_SysMouse, &mouseInput, NULL);
        if (FAILED(hr))
        {
            Log::Get()->Err("CreateDevice DI error");
            exit(Log::DIERROR);
        }
        hr = keyboardInput->SetDataFormat(&c_dfDIKeyboard);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetDataFormat DI error");
            exit(Log::DIERROR);
        }
        hr = keyboardInput->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetCooperativeLevel DI error");
            exit(Log::DIERROR);
        }
        hr = mouseInput->SetDataFormat(&c_dfDIMouse);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetDataFormat DI error");
            exit(Log::DIERROR);
        }
        hr = mouseInput->SetCooperativeLevel(hwnd, DISCL_EXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetCooperativeLevel DI error");
            exit(Log::DIERROR);
        }

        return true;
    }

    void Input::StartDetectInput()
    {
        keyboardInput->Acquire();
        mouseInput->Acquire();

        mouseInput->GetDeviceState(sizeof(DIMOUSESTATE), &mouseCurrentState);

        keyboardInput->GetDeviceState(sizeof(keyboardState), (LPVOID)&keyboardState);
    }

    void Input::EndDetectInput()
    {
        mouseLastState = mouseCurrentState;
    }

    bool Input::CheckMouseMove(int xory)
    {
        switch (xory) {
        case 1:
            return mouseCurrentState.lY != mouseLastState.lY;
        case 2:
            return mouseCurrentState.lX != mouseLastState.lX;
        default:
            return false;
        }
    }


}