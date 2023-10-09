#include "pch.h"
#include "Camera.h"


namespace BJEngine {

	float Camera::moveSpeed = 500.0f;
	float Camera::sensitivity = 0.001f;
	bool Camera::isButton = false;
	float Camera::FoV = M_PI / 2;

	Camera::Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye = eye;
		this->up = up;
		this->at = at;

		viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);
		projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);

		

		Log::Get()->Debug("Camera was created");
	}

	Camera::~Camera()
	{

	}

	void Camera::Close()
	{
		Log::Get()->Debug("Camera was closed");
	}

	void Camera::CameraMove()
	{
		Input::Get()->StartDetectInput();

		if (!isButton)
		{
			isButton = ImGui::Button("camera");
		}
		if (isButton)
		{
			ImGui::Begin("Camera");
			ImGui::SliderFloat("CameraSpeed", &moveSpeed, 10.0f, 1000.0f);
			ImGui::SliderFloat("Sensitivity", &sensitivity, 0.00001f, 0.01f);
			ImGui::SliderFloat("FoV", &FoV, 0.5f, 2.4f);

			isButton = !ImGui::Button("CloseCameraWindow");

			ImGui::End();
		}
		
		projectionMatrix = dx::XMMatrixPerspectiveFovLH(FoV, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);

		DirectX::XMVECTOR cameraDirection = DirectX::XMVector3Normalize(at - eye);
		DirectX::XMVECTOR rightDirection = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraDirection));
		
		if (Input::Get()->CheckKeyState(DIK_W))
		{
			eye += moveSpeed * 0.016f * cameraDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_S))
		{
			eye -= moveSpeed * 0.016f * cameraDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_D))
		{
			eye += moveSpeed * 0.016f * rightDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_A))
		{
			eye -= moveSpeed * 0.016f * rightDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_SPACE))
		{
			eye += moveSpeed * 0.016f * up;
		}
		if (Input::Get()->CheckKeyState(DIK_LCONTROL))
		{
			eye -= moveSpeed * 0.016f * up;
		}
		if ((Input::Get()->GetCurrState().lX != Input::Get()->GetLastState().lX) ||
			(Input::Get()->GetCurrState().lY != Input::Get()->GetLastState().lY))
		{
			camYaw += Input::Get()->GetLastState().lX * sensitivity;
		
			camPitch += Input::Get()->GetCurrState().lY * sensitivity;
		
			Input::Get()->EndDetectInput();
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

	void Camera::SetViewMatrix(dx::XMMATRIX view)
	{
		viewMatrix = view;
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