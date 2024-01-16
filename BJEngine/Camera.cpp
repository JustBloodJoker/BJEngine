#include "Camera.h"


namespace BJEngine 
{


	Camera::Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye = eye;
		this->up  = up;
		this->at  = at;
		this->camPitch = 0.0f;
		this->camYaw = 0.0f;

		viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);
		projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);

		Log::Get()->Debug("Camera was created");
	}

	Camera::Camera() : eye(dx::XMVectorSet(0.0f, 100.0f,  118.0f, 0.0f)), 
					   at(dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
					   up(up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
					   camPitch (0.0f), camYaw(0.0f), 
			           projectionMatrix(dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f)),
		               FoV(M_PI / 2), cone(nullptr), moveSpeed(1.0f), sensitivity(0.001f)
	{
		viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);


		Log::Get()->Debug("Camera created");
	}

	Camera::Camera(ID3D11Device* pd3dDevice) : eye(dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f)),
											   at(dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f)),
											   up(up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)),
											   camPitch(0.0f), camYaw(0.0f),
											   projectionMatrix(dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f)),
											   FoV(M_PI / 2), cone(nullptr), moveSpeed(1.0f), sensitivity(0.001f)
	{
		cone = new CameraCone(pd3dDevice);

		viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);

		Log::Get()->Debug("Camera created");
	}

	Camera::~Camera()
	{

	}

	void Camera::Close()
	{
		CLOSE(cone);
		Log::Get()->Debug("Camera was closed");
	}

	void Camera::CameraMove()
	{
		projectionMatrix = dx::XMMatrixPerspectiveFovLH(FoV, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);
		
		if (moveCamera)
		{
			Input::Get()->StartDetectInput();

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
			if (((Input::Get()->GetCurrState().lX != Input::Get()->GetLastState().lX) ||
				(Input::Get()->GetCurrState().lY != Input::Get()->GetLastState().lY)) &&
				Input::Get()->GetCurrState().rgbButtons[1] & 0x80)
			{

				camYaw += Input::Get()->GetLastState().lX * sensitivity;

				camPitch += Input::Get()->GetCurrState().lY * sensitivity;

				Input::Get()->EndDetectInput();
			}

			Input::Get()->EndDetectInput();
		}


		UpdateCamera();

	}

	void Camera::UpdateCamera()
	{
		at = dx::XMVector3TransformCoord(dx::XMVectorSet(0.0f, 0.0f, 0.01f, 0.0f),
			dx::XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0));
		at = dx::XMVector3Normalize(at);
		up = dx::XMVector3TransformCoord(up, dx::XMMatrixRotationY(camYaw));
		at += eye;
		viewMatrix = XMMatrixLookAtLH(eye, at, up);

		frustum.CreateFromMatrix(frustum, projectionMatrix);
		frustum.Transform(frustum, DirectX::XMMatrixInverse(nullptr, viewMatrix));
	}

	void Camera::DrawCameraObject(ID3D11DeviceContext* pImmediateContext, dx::XMMATRIX view, dx::XMMATRIX proj)
	{
		if (moveCamera)
		{
			CameraMove();
		}
		else
		{
			cone->Draw(pImmediateContext, view, proj, eye.vector4_f32[0], eye.vector4_f32[1], eye.vector4_f32[2], camYaw, camPitch, FoV);
		}
	}

	void Camera::DrawCameraObject()
	{
	}

	void Camera::SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye = eye;
		this->up = up;
		this->at = at;

		viewMatrix = XMMatrixLookAtLH(eye, at, up);
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		this->eye = dx::XMVectorSet(x, y, z, 0.0f);		
	}

	void Camera::SetViewMatrix(dx::XMMATRIX view)
	{
		viewMatrix = view;
	}

	dx::CXMMATRIX Camera::GetViewMatrix() const
	{
		return viewMatrix;
	}

	dx::CXMMATRIX Camera::GetProjectionMatrix() const
	{
		return projectionMatrix;
	}

	dx::BoundingFrustum Camera::GetFrustum() const
	{
		return frustum;
	}

	dx::XMVECTOR Camera::GetEyeVector() const
	{
		return eye;
	}


}