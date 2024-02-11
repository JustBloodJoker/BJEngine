#pragma once 
#include "pch.h"
#include "Camera.h"

namespace BJEngine
{

    
    class MainCamera :
        public Camera
    {
    public:

        MainCamera();


        void DrawCameraObject(dx::XMMATRIX view, dx::XMMATRIX proj) override;
        void DrawCameraObject() override;

    protected:
        
    };
    

}