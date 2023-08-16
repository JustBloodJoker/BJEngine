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
		void CameraMove(Input* input);
			
		void UpdateCamera();

		void SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up);

		dx::CXMMATRIX GetViewMatrix();
		dx::XMVECTOR GetEyeVector();
	private:
		dx::XMMATRIX viewMatrix;

		dx::XMVECTOR eye;
		dx::XMVECTOR at;
		dx::XMVECTOR up;


		dx::XMVECTOR DefaultForward = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		dx::XMVECTOR DefaultRight = dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);
		dx::XMVECTOR camForward = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		dx::XMVECTOR camRight = dx::XMVectorSet(1.0f, 0.0f, 0.0f, 0.0f);

		dx::XMMATRIX camRotationMatrix;
		dx::XMMATRIX groundWorld;

		float moveLeftRight = 0.0f;
		float moveBackForward = 0.0f;

		float camYaw = 0.0f;
		float camPitch = 0.0f;


	};


}