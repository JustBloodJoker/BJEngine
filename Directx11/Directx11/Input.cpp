#include "pch.h"
#include "Input.h"

namespace BJEngine {

    Input* Input::m_instance = nullptr;

    IDirectInputDevice8* Input::keyboardInput = nullptr;
    IDirectInputDevice8* Input::mouseInput = nullptr;

    BYTE Input::keyboardState[256] = {};
    DIMOUSESTATE Input::mouseCurrentState = {};
    DIMOUSESTATE Input::mouseLastState = {};
    LPDIRECTINPUT8 Input::directInput = NULL;

    Input::Input()
    {
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
        
        if (!m_instance)
        {
            m_instance = this;
        }
        else
        {
            return true;
        }

        keyboardInput = nullptr;
        mouseInput = nullptr;

        if (FAILED(hr))
        {
            Log::Get()->Err("DirectInput8Create error");
            return false;
        }

        hr = directInput->CreateDevice(GUID_SysKeyboard, &keyboardInput, NULL);
        if (FAILED(hr)) 
        {
            Log::Get()->Err("CreateDevice DI error");
            return false;
        }

        hr = directInput->CreateDevice(GUID_SysMouse, &mouseInput, NULL);
        if (FAILED(hr))
        {
            Log::Get()->Err("CreateDevice DI error");
            return false;
        }
        hr = keyboardInput->SetDataFormat(&c_dfDIKeyboard);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetDataFormat DI error");
            return false;
        }
        hr = keyboardInput->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetCooperativeLevel DI error");
            return false;
        }
        hr = mouseInput->SetDataFormat(&c_dfDIMouse);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetDataFormat DI error");
            return false;
        }
        hr = mouseInput->SetCooperativeLevel(hwnd, DISCL_NONEXCLUSIVE | DISCL_NOWINKEY | DISCL_FOREGROUND);
        if (FAILED(hr))
        {
            Log::Get()->Err("SetCooperativeLevel DI error");
            return false;
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


}