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

	void Camera::CameraMove(Input* input)
	{
		input->StartDetectInput();

		float moveSpeed = 5.0f; 
		float deltaTime = 0.016f;

		DirectX::XMVECTOR cameraDirection = DirectX::XMVector3Normalize(at - eye);
		DirectX::XMVECTOR rightDirection = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraDirection));
		DirectX::XMVECTOR upDirection = DirectX::XMVector3Cross(cameraDirection, rightDirection);

		if (input->CheckKeyState(DIK_W))
		{
			eye += moveSpeed * deltaTime * cameraDirection;
		}
		if (input->CheckKeyState(DIK_S))
		{
			eye -= moveSpeed * deltaTime * cameraDirection;
		}
		if (input->CheckKeyState(DIK_D))
		{
			eye += moveSpeed * deltaTime * rightDirection;
		}
		if (input->CheckKeyState(DIK_A))
		{
			eye -= moveSpeed * deltaTime * rightDirection;
		}
		if (input->CheckKeyState(DIK_SPACE))
		{
			eye += moveSpeed * deltaTime * up;
		}
		if (input->CheckKeyState(DIK_LCONTROL))
		{
			eye -= moveSpeed * deltaTime * up;
		}
		if ((input->GetCurrState().lX != input->GetLastState().lX) || (input->GetCurrState().lY != input->GetLastState().lY))
		{
			camYaw += input->GetLastState().lX * 0.001f;

			camPitch += input->GetCurrState().lY * 0.001f;

			input->EndDetectInput();
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