#include "pch.h"
#include "Render.h"

namespace BJEngine {


	using namespace DirectX;
	using namespace BJEUtils;

	bool Render::CreateDevice()
	{
		HRESULT hr = S_OK;

		D3D_DRIVER_TYPE driverTypes[] =
		{
			D3D_DRIVER_TYPE_HARDWARE,
			D3D_DRIVER_TYPE_WARP,
			D3D_DRIVER_TYPE_REFERENCE
		};

		UINT numDriverTypes = ARRAYSIZE(driverTypes);

		D3D_FEATURE_LEVEL featureLevels[] =
		{
			D3D_FEATURE_LEVEL_11_0,
			D3D_FEATURE_LEVEL_10_1,
			D3D_FEATURE_LEVEL_10_0,
		};
		UINT numFeatureLevels = ARRAYSIZE(featureLevels);

		DXGI_SWAP_CHAIN_DESC sd;
		ZeroMemory(&sd, sizeof(sd));
		sd.BufferCount = 1;
		sd.BufferDesc.Width = WIDTH;
		sd.BufferDesc.Height = HEIGHT;
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = !_WINDOWED;


		for (UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++) {
			driverType = driverTypes[driverTypeIndex];
			hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL,
				NULL, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
				&sd, &pSwapChain, &pd3dDevice, &featureLevel, &pImmediateContext);
			if (SUCCEEDED(hr)) {
				break;
			}
		}
		if (FAILED(hr)) {
			Log::Get()->Err("CreateDeviceAndSwapchain error");
			return false;
		}
			
		ID3D11Texture2D* pBackBuffer = NULL;
		hr = pSwapChain->GetBuffer(NULL, __uuidof(ID3D11Texture2D), (LPVOID*)&pBackBuffer);
		if (FAILED(hr)) {
			Log::Get()->Err("BackBuffer error");
			return false;
		}
			
		hr = pd3dDevice->CreateRenderTargetView(pBackBuffer, NULL, &pRenderTargetView);
		RELEASE(pBackBuffer);
		if (FAILED(hr)) {
			Log::Get()->Err("RTV create error");
			return false;
		}

		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, NULL);

		D3D11_VIEWPORT vp;
		vp.Width = WIDTH;
		vp.Height = HEIGHT;
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		pImmediateContext->RSSetViewports(1, &vp);

		return true;
	}

	void Render::BeginFrame()
	{
		float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, ClearColor);
		pImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, depthStencilView);
	}

	void Render::EndFrame()
	{
		pSwapChain->Present(0, 0);
	}

	bool Render::Init()
	{
		HRESULT hr = S_OK;
		if (!CreateDevice())
		{
			return false;
		}

		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = WIDTH;
		depthStencilDesc.Height = HEIGHT;
		depthStencilDesc.MipLevels = 1;
		depthStencilDesc.ArraySize = 1;
		depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		depthStencilDesc.SampleDesc.Count = 1;
		depthStencilDesc.SampleDesc.Quality = 0;
		depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
		depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
		depthStencilDesc.CPUAccessFlags = 0;
		depthStencilDesc.MiscFlags = 0;

		hr = pd3dDevice->CreateTexture2D(&depthStencilDesc, NULL, &depthStencilBuffer);
		if (FAILED(hr)) {
			Log::Get()->Err("Create Depth buffer error");
			return false;
		}

		hr = pd3dDevice->CreateDepthStencilView(depthStencilBuffer, NULL, &depthStencilView);
		if (FAILED(hr)) {
			Log::Get()->Err("Create DepthStencil buffer error");
			return false;
		}

		cam = new Camera(dx::XMVectorSet(0.0f, 5.0f, -8.0f, 0.0f),
			dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		sound = new BJAudio::Sound();

		Projection = dx::XMMatrixPerspectiveFovLH(0.4f * 3.14f, WIDTH / HEIGHT, 1.0f, 1000.0f);

		InitIsLightConstantBuffer();

		return true;
	}

	bool Render::DrawWnd()
	{
		BeginFrame();
		GetCamera()->CameraMove();

		Draw();
		if(islight)
			light->DrawLight(pImmediateContext);

		DrawIsLightConstantBuffer();
		
		EndFrame();

		return true;
	}

	Object* Render::InitObjs(Object* object)
	{
		object->SetCamera(cam);
		object->SetDevice(pd3dDevice);
		object->SetDeviceContext(pImmediateContext);

		if (!object->Init()) {
			return nullptr;
		};

		return object;
	}

	bool Render::Draw()
	{
		BeginFrame();
		pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		//sound->InitAndPlay(TEXT("892dc731-9cb3-4c48-8a11-0f7316e2b3ed.wav"));

		cam->CameraMove();



		EndFrame();
		
		return true;
	}

	void Render::SetLight(LightDesc* ld, int typeOfLight)
	{
		Light* spl = new Light(ld);

		switch(typeOfLight)
		{
		case SPOTLIGHT:
			istypeoflight = { false };
			istypeoflight.isSpotLight = true;
			break;

		case DIRECTIONALLIGHT:
			istypeoflight = { false };
			istypeoflight.isDirLight = true;
			break;
		
		case POINTLIGHT:
			istypeoflight = { false };
			istypeoflight.isPointLight = true;
			break;
		}
		islight = true;
		spl->InitLight(pd3dDevice);

		light = spl;
	}

	bool Render::InitIsLightConstantBuffer()
	{
		ilcb = Object::InitConstantBuffer<IsLightsConstantBuffer>(pd3dDevice);

		if (ilcb != nullptr)
			return true;
		else
			return false;
	}
	
	void Render::DrawIsLightConstantBuffer()
	{
		pImmediateContext->UpdateSubresource(ilcb, 0, NULL, &istypeoflight, 0, 0);
		pImmediateContext->PSSetConstantBuffers(1, 1, &ilcb);

	}

	void Render::Close()
	{
		CLOSE(sound);
		CLOSE(cam);
		LCLOSE(light);
		RELEASE(ilcb);
		RELEASE(depthStencilView);
		RELEASE(depthStencilBuffer);
		RELEASE(pd3dDevice);
		if (pImmediateContext)
			pImmediateContext->ClearState();
		RELEASE(pImmediateContext);
		RELEASE(pSwapChain);
		RELEASE(pRenderTargetView);

		Log::Get()->Debug("Render was closed");
	}

	


}