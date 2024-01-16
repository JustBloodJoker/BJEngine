#pragma once
#include "pch.h"
#include "CameraCone.h"

namespace BJEngine {


	class Camera
	{

		friend class UI;

	public:

		Camera(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up);
		Camera();
		Camera(ID3D11Device* pd3dDevice);

		virtual ~Camera();

		virtual void Close();

		virtual void DrawCameraObject(ID3D11DeviceContext* pImmediateContext, dx::XMMATRIX view, dx::XMMATRIX proj);
		virtual void DrawCameraObject();
		
		void SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up);
		void SetPosition(float x, float y, float z);
		void SetViewMatrix(dx::XMMATRIX view);

		dx::CXMMATRIX GetViewMatrix() const;
		dx::CXMMATRIX GetProjectionMatrix() const;
		dx::BoundingFrustum GetFrustum() const;
		dx::XMVECTOR GetEyeVector() const;

	protected:

		void CameraMove();

		void UpdateCamera();

		dx::XMMATRIX viewMatrix;
		dx::XMMATRIX projectionMatrix;

		dx::XMVECTOR eye;
		dx::XMVECTOR at;
		dx::XMVECTOR up;

		float camYaw;
		float camPitch;
		float FoV;
		float moveSpeed;
		float sensitivity;
		bool moveCamera;

		dx::BoundingFrustum frustum;
	private:

		CameraCone* cone;

		
	};


}