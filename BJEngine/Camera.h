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
		void CameraMove();

		void UpdateCamera();

		void SetPosition(dx::XMVECTOR eye, dx::XMVECTOR at, dx::XMVECTOR up);
		void SetViewMatrix(dx::XMMATRIX view);

		dx::CXMMATRIX GetViewMatrix();
		dx::CXMMATRIX GetProjectionMatrix();
		dx::BoundingFrustum GetFrustum();
		dx::XMVECTOR GetEyeVector();

	private:
		void CloneCamera();
		void CreateCamera();

		dx::XMMATRIX viewMatrix;
		dx::XMMATRIX projectionMatrix;

		int cameraNumbers = 0;
		int cameraInputNum = 0;

		std::vector <dx::XMVECTOR>	eye;
		std::vector < dx::XMVECTOR> at;
		std::vector < dx::XMVECTOR> up;

		std::vector<float> camYaw;
		std::vector<float> camPitch;

		float moveSpeed = 500.0f;
		float sensitivity = 0.001f;
		bool isButton = false;
		float FoV = M_PI / 2;


		float x1, y1, z1, w1;
		float x2, y2, z2, w2;
		float x3, y3, z3, w3;

		bool isB = false;

		dx::BoundingFrustum frustum;
	};


}