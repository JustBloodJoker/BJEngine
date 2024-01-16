#include "Render.h"
#include "UI.h"
#include "LightMananger.h"
#include "Model.h"


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
		pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, DepthStencil::ClearDepthStencil());

		UI::Begin();
	}

	void Render::EndFrame()
	{
		UI::End();

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

		DepthStencil::InitStencils(pRenderTargetView, pd3dDevice, pImmediateContext);

		cams.push_back(new MainCamera());
		if (!UI::Init(hwnd, pd3dDevice, pImmediateContext, cams, this))
		{
			Log::Get()->Err("Incorrect ImGui init!");
		}
		else
		{
			Log::Get()->Debug("ImGui was inited!");
		}

		if (!LightMananger::IsInited())
		{
			LightMananger::Init(pd3dDevice);
		}

		return true;
	}

	bool Render::DrawWnd()
	{
		if (GetIsResizedState())
		{
			ResizeWindow();
		}

		float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, ClearColor);

		LightMananger::DrawShadows(pImmediateContext, objects);

		BeginFrame();
		
		UnpackProject();
		
		cams[0]->DrawCameraObject();

		for (auto& el : cams)
		{
			el->DrawCameraObject(pImmediateContext, cams[UI::FocusedCamera()]->GetViewMatrix(), cams[UI::FocusedCamera()]->GetProjectionMatrix());
		}

		for (auto& tSound : sound)
		{
			tSound->Play();
		}
	
		if (LightMananger::IsInited())
			LightMananger::Draw(pImmediateContext);

		for (auto& object : objects)
		{
			object->SetCamera(cams[UI::FocusedCamera()]);
			LightMananger::SetMatrix(object);
		}

		
		pImmediateContext->PSSetSamplers(0, 1, Textures::GetBorderState());

		for (auto& object : objects)
		{
			object->Draw();
		}

		if (skyBox && skyBox->IsInited())
		{
			skyBox->SetCamera(cams[UI::FocusedCamera()]);
			skyBox->Draw();
		}

	//	if (Input::Get()->CheckKeyState(DIK_8))
	//	{
	//		ID3D11Texture2D* pBuffer;
	//
	//		pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&pBuffer);
	//
	//		ID3D11Texture2D* texture_to_save = nullptr;
	//		if (texture_to_save == nullptr)
	//		{
	//			D3D11_TEXTURE2D_DESC td;
	//			pBuffer->GetDesc(&td);
	//			pd3dDevice->CreateTexture2D(&td, NULL, &texture_to_save);
	//		}
	//
	//		pImmediateContext->CopyResource(texture_to_save, pBuffer);
	//
	//		D3DX11SaveTextureToFile(pImmediateContext, texture_to_save, D3DX11_IFF_PNG, L"ffddss.png");
	//	}


		EndFrame();

		if (PackMananger::Get()->GetSavingStatus())
		{
			PackMananger::Get()->Close();
		}
		
		if (UnpackMananger::Get()->GetOpeningStatus())
		{
			UnpackMananger::Get()->Reset();
		}

		return true;
	}

	Object* Render::InitObjs(Object* object)
	{

		if (!object->IsInited())
		{
			object->SetDevice(pd3dDevice);
			object->SetDeviceContext(pImmediateContext);

			if (!object->Init()) {
				return nullptr;
			};
		}
		objects.push_back(object);

		return object;
	}

	void Render::AddSkyBox(std::string texturePath)
	{
		if (skyBox)
		{
			CLOSE(skyBox);
		}
		skyBox = new BackGround();
		skyBox->SetTexture(new Textures(std::wstring(texturePath.begin(), texturePath.end())));
		if (!skyBox->IsInited())
		{
			skyBox->SetDevice(pd3dDevice);
			skyBox->SetDeviceContext(pImmediateContext);
			if (!skyBox->Init())
			{
				Log::Get()->Err("SkyBox didn't inited!");
			}
		}

	}

	void Render::ResizeWindow()
	{
		pSwapChain->SetFullscreenState(GetFullScreenState(), nullptr);

		vp.Width = GetWindowWidth();
		vp.Height = GetWindowHeight();
	}

	void Render::CreateSound(std::string path)
	{
		sound.push_back(new BJAudio::Sound(std::wstring(path.begin(), path.end())));
		sound[sound.size() - 1]->Init();
	}

	void Render::UnpackProject()
	{
		

		if (UnpackMananger::Get()->GetOpeningStatus())
		{
			std::vector<LightType> unp = UnpackMananger::Get()->GetLights();

			for (auto& el : unp)
			{
				ld.pos = el.pos;
				ld.dir = el.dir;
				ld.angle = el.angle;
				ld.att = el.att;
				ld.color = el.color;
				ld.enabled = el.enabled;
				ld.lightType = el.lightType;
				SetLight(ld);
			}

			unp.clear();
	
			std::vector<ObjectType> unpo = UnpackMananger::Get()->GetObject();
			Model* md = nullptr;
			for (auto& el : unpo)
			{
				md = new Model(el.path.c_str());
				md->SetTexturesPrefixPath(std::wstring(el.prepath.begin(), el.prepath.end()));
				md->SetScript(el.script);
				InitObjs(md);
			}
		}


	}

	void Render::SetLight(LightDesc ld)
	{
		if (LightMananger::AddLight(ld))
		{
			
		};
		islight = true;


		
	}

	void Render::Close()
	{
		for (auto& el : objects) {
			CLOSE(el);
		}
		objects.clear();

		for (auto& el : cams)
		{
			CLOSE(el);
		}
		cams.clear();

		
		RELEASE(pd3dDevice);
		if (pImmediateContext)
			pImmediateContext->ClearState();
		RELEASE(pImmediateContext);
		RELEASE(pSwapChain);
		RELEASE(pRenderTargetView);
		Blend::Get()->Close();
		Log::Get()->Debug("Render was closed");
		PackMananger::Get()->Close();
		UnpackMananger::Get()->Close();
		LightMananger::Close();
	}


}