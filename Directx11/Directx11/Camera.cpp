#include "pch.h"
#include "Camera.h"


namespace BJEngine {


	Camera::Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye = eye;
		this->up = up;
		this->at = at;

		viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);
		Log::Get()->Debug("Create camera");
	}

	Camera::~Camera()
	{

	}

	void Camera::Close()
	{
		Log::Get()->Debug("Closing camera");
	}

	void Camera::CameraMove()
	{
		Input::Get()->StartDetectInput();

		float moveSpeed = 5.0f; 
		float deltaTime = 0.016f;

		DirectX::XMVECTOR cameraDirection = DirectX::XMVector3Normalize(at - eye);
		DirectX::XMVECTOR rightDirection = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraDirection));
		DirectX::XMVECTOR upDirection = DirectX::XMVector3Cross(cameraDirection, rightDirection);

		if (Input::Get()->CheckKeyState(DIK_W))
		{
			eye += moveSpeed * deltaTime * cameraDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_S))
		{
			eye -= moveSpeed * deltaTime * cameraDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_D))
		{
			eye += moveSpeed * deltaTime * rightDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_A))
		{
			eye -= moveSpeed * deltaTime * rightDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_SPACE))
		{
			eye += moveSpeed * deltaTime * up;
		}
		if (Input::Get()->CheckKeyState(DIK_LCONTROL))
		{
			eye -= moveSpeed * deltaTime * up;
		}
		//if ((Input::GetCurrState().lX != Input::GetLastState().lX) || (Input::GetCurrState().lY != Input::GetLastState().lY))
		//{
		//	camYaw += Input::GetLastState().lX * 0.001f;
		//
		//	camPitch += Input::GetCurrState().lY * 0.001f;
		//
		//	Input::EndDetectInput();
		//}
		if (Input::Get()->CheckKeyState(DIK_LEFT))
		{
			camYaw -= 0.03f;
		}
		if (Input::Get()->CheckKeyState(DIK_RIGHT))
		{
			camYaw += 0.03f;
		}
		if (Input::Get()->CheckKeyState(DIK_UP))
		{
			camPitch -= 0.03f;
		}
		if (Input::Get()->CheckKeyState(DIK_DOWN))
		{
			camPitch += 0.03f;
		}

		UpdateCamera();
	}

	void Camera::UpdateCamera()
	{
		at = dx::XMVector3TransformCoord(dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			dx::XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0));
		at = dx::XMVector3Normalize(at);
		up = dx::XMVector3TransformCoord(up, dx::XMMatrixRotationY(camYaw));
		at += eye;
		viewMatrix = XMMatrixLookAtLH(eye, at, up);
	}

	void Camera::SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye = eye;
		this->up = up;
		this->at = at;

		viewMatrix = XMMatrixLookAtLH(eye, at, up);
	}


	dx::CXMMATRIX Camera::GetViewMatrix()
	{
		return viewMatrix;
	}

	dx::XMVECTOR Camera::GetEyeVector()
	{
		return eye;
	}


}