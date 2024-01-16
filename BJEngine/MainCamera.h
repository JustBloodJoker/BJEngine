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
       // void Close() override;


        void DrawCameraObject(ID3D11DeviceContext* pImmediateContext, dx::XMMATRIX view, dx::XMMATRIX proj) override;
        void DrawCameraObject() override;

    protected:
        
    };
    

}