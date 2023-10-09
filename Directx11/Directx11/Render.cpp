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
		sd.BufferDesc.Width = BJEUtils::GetWindowWidth();
		sd.BufferDesc.Height = BJEUtils::GetWindowHeight();
		sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		sd.BufferDesc.RefreshRate.Numerator = 60;
		sd.BufferDesc.RefreshRate.Denominator = 1;
		sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		sd.OutputWindow = hwnd;
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
		sd.Windowed = !GetFullScreenState();

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

		
		vp.Width = BJEUtils::GetWindowWidth();
		vp.Height = BJEUtils::GetWindowHeight();
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
		pImmediateContext->RSSetViewports(1, &vp);
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, ClearColor);
		pImmediateContext->ClearDepthStencilView(depthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, depthStencilView);

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();

		ImGui::NewFrame();
		static bool isBtoF = GetFullScreenState();
		ImGui::Checkbox("FullScreen", &isBtoF);
		SetFullScreenState(isBtoF);
	}

	void Render::EndFrame()
	{
		ImGui::Render();
		ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

		pSwapChain->Present(0, 0);
	}

	bool Render::Init()
	{
		HRESULT hr = S_OK;
		if (!CreateDevice())
		{
			return false;
		}

		Blend* blend = new Blend(pd3dDevice);
		
		D3D11_TEXTURE2D_DESC depthStencilDesc;
		depthStencilDesc.Width = BJEUtils::GetWindowWidth();
		depthStencilDesc.Height = BJEUtils::GetWindowHeight();
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

		cam = new Camera(dx::XMVectorSet(0.0f, 100.0f, -8.0f, 0.0f),
			dx::XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			dx::XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f));

		sound = new BJAudio::Sound();
	
		
		InitImGui();

		return true;
	}

	int o = 1;

	bool Render::DrawWnd()
	{	
		if (GetIsResizedState())
		{
			ResizeWindow();
		}

		float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, ClearColor);


		for (size_t index = 0; index < shadows.size(); index++)
		{
			shadows[index]->Render(pImmediateContext, light->GetDesc(index), objects);
		}
		
		BeginFrame();
		ImGui::Text("conbawa!");

		GetCamera()->CameraMove();
		//sound->InitAndPlay(TEXT("892dc731-9cb3-4c48-8a11-0f7316e2b3ed.wav"));
		
		if (islight)
			light->DrawLight(pImmediateContext);

		

		for (auto& object : objects)
		{
			for (int index = 0; index < shadows.size(); index++)
			{
				object->SetLightViewAndProjectionMatrix(shadows[index]->GetView(), shadows[index]->GetProjection(), index);
			}
			object->SetViewAndProjectionMatrix(GetCamera()->GetViewMatrix(), GetCamera()->GetProjectionMatrix());
		}

		for (int index = 0; index < shadows.size(); index++)
		{
			if(light->GetDesc(index).lightType == POINTLIGHT)
				pImmediateContext->PSSetShaderResources(2 + index, 1 , shadows[index]->GetTexture());
			else 
				pImmediateContext->PSSetShaderResources(2 + index + 5, 1, shadows[index]->GetTexture());
		}
		pImmediateContext->PSSetSamplers(0, 1, Textures::GetBorderState());

		DrawActions();

		EndFrame();

		return true;
	}

	Object* Render::InitObjs(Object* object)
	{

		if (!object->IsInited())
		{
			object->SetCamera(cam);
			object->SetDevice(pd3dDevice);
			object->SetDeviceContext(pImmediateContext);

			if (!object->Init()) {
				return nullptr;
			};
		}
		objects.push_back(object);

		return object;
	}

	void Render::ResizeWindow()
	{

		pSwapChain->SetFullscreenState(GetFullScreenState(), nullptr);

		vp.Width = GetWindowWidth();
		vp.Height = GetWindowHeight();
	}

	bool Render::DrawActions()
	{
		
		return true;
	}

	void Render::SetLightPos(float x, float y, float z, int indexOfLight)
	{
		light->SetPos(x, y, z, indexOfLight);
	}

	dx::XMFLOAT3 Render::GetLightPos(int index)
	{
		return light->GetPos(index);
	}

	void Render::InitImGui()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO io = ImGui::GetIO();
		ImGui_ImplWin32_Init(hwnd);
		ImGui_ImplDX11_Init(pd3dDevice, pImmediateContext);
		ImGui::StyleColorsDark();
	}

	void Render::DrawImGui()
	{
		
		
		

		
	}

	void Render::SetLight(LightDesc* ld, int typeOfLight)
	{
		ld->lightType = typeOfLight;
		if (light == nullptr)
		{
			light = new Light(ld);
		} 
		else
		{
			light->SetLightDesc(ld);
		}
		islight = true;

		if (!isInitlight)
		{
			light->InitLight(pd3dDevice);
			isInitlight = true;
		}
		shadows.push_back(new Shadow());
		shadows[shadows.size() - 1]->InitShadow(pd3dDevice, ld->lightType);

	}

	void Render::Close()
	{

		for (auto& el : objects) {
			CLOSE(el);
		}
		objects.clear();

		CLOSE(sound);
		CLOSE(cam);
		LCLOSE(light);
		RELEASE(depthStencilView);
		RELEASE(depthStencilBuffer);
		RELEASE(pd3dDevice);
		if (pImmediateContext)
			pImmediateContext->ClearState();
		RELEASE(pImmediateContext);
		RELEASE(pSwapChain);
		RELEASE(pRenderTargetView);
		Blend::Get()->Close();
		Log::Get()->Debug("Render was closed");

		
	}


}