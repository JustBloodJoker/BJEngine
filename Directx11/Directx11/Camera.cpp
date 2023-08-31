#include "pch.h"
#include "Camera.h"


namespace BJEngine {


	Camera::Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye = eye;
		this->up = up;
		this->at = at;

		viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);
		projectionMatrix = dx::XMMatrixPerspectiveFovLH(0.4f * 3.14f, WIDTH / HEIGHT, 1.0f, 1000.0f);

		

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
		//if ((Input::Get()->GetCurrState().lX != Input::Get()->GetLastState().lX) ||
		//	(Input::Get()->GetCurrState().lY != Input::Get()->GetLastState().lY))
		//{
		//	camYaw += Input::Get()->GetLastState().lX * 0.001f;
		//
		//	camPitch += Input::Get()->GetCurrState().lY * 0.001f;
		//
		//	Input::Get()->EndDetectInput();
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

		frustum.CreateFromMatrix(frustum,projectionMatrix);
		frustum.Transform(frustum, DirectX::XMMatrixInverse(nullptr, viewMatrix));
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

	dx::CXMMATRIX Camera::GetProjectionMatrix()
	{
		return projectionMatrix;
	}

	dx::BoundingFrustum Camera::GetFrustum()
	{
		return frustum;
	}

	dx::XMVECTOR Camera::GetEyeVector()
	{
		return eye;
	}


}