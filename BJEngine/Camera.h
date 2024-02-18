#pragma once
#include "pch.h"
#include "CameraCone.h"

namespace BJEngine {

	struct CameraDesc
	{
		dx::XMMATRIX viewMatrix;
		dx::XMMATRIX projectionMatrix;
		dx::BoundingFrustum frustum;

		dx::XMVECTOR eye;
		dx::XMVECTOR at;
		dx::XMVECTOR up;

		void GenFrustum()
		{
			frustum.CreateFromMatrix(frustum, projectionMatrix);
			frustum.Transform(frustum, DirectX::XMMatrixInverse(nullptr, viewMatrix));
		};

	};

	class Camera
	{

		friend class UI;
		static ID3D11Buffer* cameraBuffer;

	public:

		Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up, float aspectRatio);
		Camera();
		Camera(ID3D11Device* pd3dDevice);

		virtual ~Camera();

		virtual void Close();

		virtual void DrawCameraObject(dx::XMMATRIX view, dx::XMMATRIX proj);
		virtual void DrawCameraObject();
		
		void SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up);
		void SetPosition(float x, float y, float z);
		void SetViewMatrix(dx::XMMATRIX view);

		CameraDesc GetDesc() const noexcept;

		
		static void SetCameraBuffer(const CameraDesc ds);


	protected:
		static void InitCameraBuffer();

		void CameraMove();

		void UpdateCamera();

		CameraDesc camDesc;

		float camYaw;
		float camPitch;
		float FoV;
		float moveSpeed;
		float sensitivity;
		bool moveCamera;

	private:

		CameraCone* cone = nullptr;

		
	};


}