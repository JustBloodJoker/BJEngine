 #pragma once
#include "pch.h"
#include "Input.h"

namespace BJEngine {


	class Camera
	{
	public:
		Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up);
		~Camera();

		void Close();
		void CameraMove();
			
		void UpdateCamera();

		void SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up);

		dx::CXMMATRIX GetViewMatrix();
		dx::XMVECTOR GetEyeVector();
	private:
		dx::XMMATRIX viewMatrix;

		dx::XMVECTOR eye;
		dx::XMVECTOR at;
		dx::XMVECTOR up;
		
		float camYaw = 0.0f;
		float camPitch = 0.0f;
	};


}