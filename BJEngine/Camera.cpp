#include "Camera.h"


namespace BJEngine 
{


	Camera::Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up, float aspectRatio)
	{
		camDesc.eye = eye;
		camDesc.up  = up;
		camDesc.at  = at;
		this->camPitch = 0.0f;
		this->camYaw = 0.0f;

		
		camDesc.viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);
		camDesc.projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, aspectRatio, 1.0f, 10000.0f);

		
		Log::Get()->Debug("Camera was created");
	}

	Camera::Camera() : camPitch (0.0f), camYaw(0.0f),
		               FoV(M_PI / 2), cone(nullptr), moveSpeed(1.0f), sensitivity(0.001f)
	{
		camDesc.eye = dx::XMVectorSet(0.0f, 100.0f, 118.0f, 0.0f);
		camDesc.at = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		camDesc.up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);
		
		camDesc.viewMatrix = dx::XMMatrixLookAtLH(camDesc.eye, camDesc.at, camDesc.up);
		camDesc.projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);

		Log::Get()->Debug("Camera created");
	}

	Camera::Camera(ID3D11Device* pd3dDevice) : 
											   camPitch(0.0f), camYaw(0.0f),
											   FoV(M_PI / 2), cone(nullptr), moveSpeed(1.0f), sensitivity(0.001f)
	{
		cone = new CameraCone();

		camDesc.eye = dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f);
		camDesc.at = dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
		camDesc.up = dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f);

		camDesc.viewMatrix = dx::XMMatrixLookAtLH(camDesc.eye, camDesc.at, camDesc.up);
		camDesc.projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);

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
		camDesc.projectionMatrix = dx::XMMatrixPerspectiveFovLH(FoV, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);
		
		if (moveCamera)
		{
			Input::Get()->StartDetectInput();

			DirectX::XMVECTOR cameraDirection = DirectX::XMVector3Normalize(camDesc.at - camDesc.eye);
			DirectX::XMVECTOR rightDirection = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraDirection));

			if (Input::Get()->CheckKeyState(DIK_W))
			{
				camDesc.eye += moveSpeed * 0.016f * cameraDirection;
			}
			if (Input::Get()->CheckKeyState(DIK_S))
			{
				camDesc.eye -= moveSpeed * 0.016f * cameraDirection;
			}
			if (Input::Get()->CheckKeyState(DIK_D))
			{
				camDesc.eye += moveSpeed * 0.016f * rightDirection;
			}
			if (Input::Get()->CheckKeyState(DIK_A))
			{
				camDesc.eye -= moveSpeed * 0.016f * rightDirection;
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
		camDesc.at = dx::XMVector3TransformCoord(dx::XMVectorSet(0.0f, 0.0f, 0.01f, 0.0f),
			dx::XMMatrixRotationRollPitchYaw(camPitch, camYaw, 0));
		camDesc.at = dx::XMVector3Normalize(camDesc.at);
		camDesc.up = dx::XMVector3TransformCoord(camDesc.up, dx::XMMatrixRotationY(camYaw));
		camDesc.at += camDesc.eye;
		camDesc.viewMatrix = XMMatrixLookAtLH(camDesc.eye, camDesc.at, camDesc.up);

		camDesc.GenFrustum();
	}

	void Camera::DrawCameraObject(dx::XMMATRIX view, dx::XMMATRIX proj)
	{
		if (moveCamera)
		{
			CameraMove();
		}
		else
		{
			cone->Draw(view, proj, camDesc.eye.vector4_f32[0], camDesc.eye.vector4_f32[1], camDesc.eye.vector4_f32[2], camYaw, camPitch, FoV);
		}
	}

	void Camera::DrawCameraObject()
	{
	}

	void Camera::SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		camDesc.eye = eye;
		camDesc.up = up;
		camDesc.at = at;

		camDesc.viewMatrix = XMMatrixLookAtLH(eye, at, up);
	}

	void Camera::SetPosition(float x, float y, float z)
	{
		camDesc.eye = dx::XMVectorSet(x, y, z, 0.0f);
	}

	void Camera::SetViewMatrix(dx::XMMATRIX view)
	{
		camDesc.viewMatrix = view;
	}

	CameraDesc Camera::GetDesc() const noexcept
	{
		return camDesc;
	}

	ID3D11Buffer* Camera::cameraBuffer = nullptr;

	void Camera::InitCameraBuffer()
	{
		
			cameraBuffer = Helper::InitConstantBuffer<BJEStruct::CameraConstantBuffer>(GP::GetDevice());
	}

	void Camera::SetCameraBuffer(const CameraDesc ds)
	{
		if (cameraBuffer == nullptr)
			InitCameraBuffer();

		BJEStruct::CameraConstantBuffer cds;
		
		cds.eyeMatrix = dx::XMMatrixTranspose(ds.viewMatrix);
		cds.eyePos = dx::XMFLOAT4(cds.eyeMatrix(0, 3), cds.eyeMatrix(1, 3), cds.eyeMatrix(2, 3), cds.eyeMatrix(3, 3));
		
		GP::GetDeviceContext()->PSSetConstantBuffers(1, 1, &cameraBuffer);
		GP::GetDeviceContext()->UpdateSubresource(cameraBuffer, 0, NULL, &cds, 0, 0);
	}

}