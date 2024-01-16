#include "MainCamera.h"

BJEngine::MainCamera::MainCamera()
{
	moveSpeed = 10.0f;
	moveCamera = true;
}

void BJEngine::MainCamera::DrawCameraObject(ID3D11DeviceContext* pImmediateContext, dx::XMMATRIX view, dx::XMMATRIX proj)
{

}

void BJEngine::MainCamera::DrawCameraObject()
{
	if (moveCamera)
	{
		ImGui::Begin("MC");

		ImGui::SliderFloat("   ", &moveSpeed, 0.1f, 1000.0f);

		ImGui::End();

		CameraMove();
	}
}
