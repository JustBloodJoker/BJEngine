#include "pch.h"
#include "Camera.h"


namespace BJEngine 
{


	Camera::Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye.push_back(eye);
		this->up.push_back(up);
		this->at.push_back(at);
		this->camPitch.push_back(0.0f);
		this->camYaw.push_back(0.0f);
		cameraNumbers++;

		viewMatrix = dx::XMMatrixLookAtLH(eye, at, up);
		projectionMatrix = dx::XMMatrixPerspectiveFovLH(M_PI / 2.0f, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);

		Log::Get()->Debug("Camera was created");
	}

	Camera::~Camera()
	{

	}

	void Camera::Close()
	{
		eye.clear();
		at.clear();
		up.clear();
		camYaw.clear();
		camPitch.clear();

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

			static char forcombo = '1';
			static char tempCameraInput = '1';
			static char tFor = '-1';
			if (ImGui::BeginCombo("Camera number", &tempCameraInput))
			{

				for (int n = 0; n < cameraNumbers; n++)
				{
					tFor = forcombo + n;
					bool is_selected = (tempCameraInput == tFor);
					if (ImGui::Selectable(&tFor, is_selected)) { tempCameraInput = tFor; }

					if (is_selected)
						ImGui::SetItemDefaultFocus();
				}
				ImGui::EndCombo();
			}
			cameraInputNum = tempCameraInput - 49;

			if (ImGui::Button("CloneCamera"))
			{
				CloneCamera();
			};

			if (!isB)
				isB = ImGui::Button("Create Camera");

			if (isB)
			{
				CreateCamera();
			}

			isButton = !ImGui::Button("CloseCameraWindow");
			ImGui::End();
		}

		projectionMatrix = dx::XMMatrixPerspectiveFovLH(FoV, BJEUtils::GetWindowWidth() / BJEUtils::GetWindowHeight(), 1.0f, 10000.0f);

		DirectX::XMVECTOR cameraDirection = DirectX::XMVector3Normalize(at[cameraInputNum] - eye[cameraInputNum]);
		DirectX::XMVECTOR rightDirection = DirectX::XMVector3Normalize(DirectX::XMVector3Cross(DirectX::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f), cameraDirection));

		if (Input::Get()->CheckKeyState(DIK_W))
		{
			eye[cameraInputNum] += moveSpeed * 0.016f * cameraDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_S))
		{
			eye[cameraInputNum] -= moveSpeed * 0.016f * cameraDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_D))
		{
			eye[cameraInputNum] += moveSpeed * 0.016f * rightDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_A))
		{
			eye[cameraInputNum] -= moveSpeed * 0.016f * rightDirection;
		}
		if (Input::Get()->CheckKeyState(DIK_SPACE))
		{
			eye[cameraInputNum] += moveSpeed * 0.016f * up[cameraInputNum];
		}
		if (Input::Get()->CheckKeyState(DIK_LCONTROL))
		{
			eye[cameraInputNum] -= moveSpeed * 0.016f * up[cameraInputNum];
		}
		if ((Input::Get()->GetCurrState().lX != Input::Get()->GetLastState().lX) ||
			(Input::Get()->GetCurrState().lY != Input::Get()->GetLastState().lY))
		{
			camYaw[cameraInputNum] += Input::Get()->GetLastState().lX * sensitivity;

			camPitch[cameraInputNum] += Input::Get()->GetCurrState().lY * sensitivity;

			Input::Get()->EndDetectInput();
		}

		UpdateCamera();

		if (PackMananger::Get()->GetSavingStatus())
		{
			CameraType* templt = new CameraType();
			for (size_t iter = 0; iter < eye.size(); iter++)
			{
				templt->camPitch = camPitch[iter];
				templt->camYaw = camYaw[iter];
				templt->FoV = FoV;
				templt->sensitivity = sensitivity;
				templt->moveSpeed = moveSpeed;
				templt->at = at[iter];
				templt->eye = eye[iter];
				templt->up = eye[iter];
				PackMananger::Get()->AddCamera(*templt);
			}

			delete templt;
		};

		if (UnpackMananger::Get()->GetOpeningStatus())
		{
			std::vector<CameraType> unp = UnpackMananger::Get()->GetCamera();

			

			for (auto& el : unp)
			{
				camPitch.push_back(el.camPitch);
				camYaw.push_back(el.camYaw);
				FoV = el.FoV;
				sensitivity = el.sensitivity;
				moveSpeed = el.moveSpeed;
				at.push_back(el.at);
				eye.push_back(el.eye);
				up.push_back(el.up);
				cameraNumbers++;
			}

		}
	}

	void Camera::UpdateCamera()
	{
		at[cameraInputNum] = dx::XMVector3TransformCoord(dx::XMVectorSet(0.0f, 0.0f, 1.0f, 0.0f),
			dx::XMMatrixRotationRollPitchYaw(camPitch[cameraInputNum], camYaw[cameraInputNum], 0));
		at[cameraInputNum] = dx::XMVector3Normalize(at[cameraInputNum]);
		up[cameraInputNum] = dx::XMVector3TransformCoord(up[cameraInputNum], dx::XMMatrixRotationY(camYaw[cameraInputNum]));
		at[cameraInputNum] += eye[cameraInputNum];
		viewMatrix = XMMatrixLookAtLH(eye[cameraInputNum], at[cameraInputNum], up[cameraInputNum]);

		frustum.CreateFromMatrix(frustum, projectionMatrix);
		frustum.Transform(frustum, DirectX::XMMatrixInverse(nullptr, viewMatrix));
	}

	void Camera::SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up)
	{
		this->eye[cameraInputNum] = eye;
		this->up[cameraInputNum] = up;
		this->at[cameraInputNum] = at;

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
		return eye[cameraInputNum];
	}

	void Camera::CloneCamera()
	{
		this->eye.push_back(eye[cameraInputNum]);
		this->up.push_back(up[cameraInputNum]);
		this->at.push_back(at[cameraInputNum]);
		this->camPitch.push_back(camPitch[cameraInputNum]);
		this->camYaw.push_back(camYaw[cameraInputNum]);
		cameraNumbers++;
	}

	void Camera::CreateCamera()
	{
		ImGui::Begin("Camera Creator");


		if (ImGui::Button("Cancel creating"))
		{
			isB = false;
		}


		dx::XMVECTOR eye, up, at;

		ImGui::Text("Eye vector");
		ImGui::InputFloat("x", &x1);
		ImGui::InputFloat("y", &y1);
		ImGui::InputFloat("z", &z1);
		ImGui::InputFloat("w", &w1);
		eye = dx::XMVectorSet(x1, y1, z1, w1);

		ImGui::Text("Up vector");
		ImGui::InputFloat("x", &x2);
		ImGui::InputFloat("y", &y2);
		ImGui::InputFloat("z", &z2);
		ImGui::InputFloat("w", &w2);
		up = dx::XMVectorSet(x2, y2, z2, w2);

		ImGui::Text("Up vector");
		ImGui::InputFloat("x", &x3);
		ImGui::InputFloat("y", &y3);
		ImGui::InputFloat("z", &z3);
		ImGui::InputFloat("w", &w3);
		at = dx::XMVectorSet(x3, y3, z3, w3);

		if (ImGui::Button("Create camera"))
		{
			this->eye.push_back(eye);
			this->up.push_back(up);
			this->at.push_back(at);
			this->camPitch.push_back(0.0f);
			this->camYaw.push_back(0.0f);
			cameraNumbers++;
			isB = false;
		}


		ImGui::End();

	}

}