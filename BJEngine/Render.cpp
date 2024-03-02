#include "Render.h"
#include "UI.h"
#include "Model.h"


namespace BJEngine 
{


	using namespace BJEUtils;

	bool Render::CreateDevice()
	{
		int MSAASupport = 16;
		D3D_DRIVER_TYPE driverType = D3D_DRIVER_TYPE_NULL;
		D3D_FEATURE_LEVEL featureLevel = D3D_FEATURE_LEVEL_11_0;

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

			hr = D3D11CreateDevice(NULL, driverType, NULL,
				NULL, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
				&GP::GetDevice(), &featureLevel, &GP::GetDeviceContext());

			if (SUCCEEDED(hr)) {
				
				
				while (MSAASupport != 1)
				{
					UINT MsaaQuality;
					hr = GP::GetDevice()->CheckMultisampleQualityLevels(
						DXGI_FORMAT_R8G8B8A8_UNORM, MSAASupport, &MsaaQuality);
					if (MsaaQuality > 0)
					{
					//	sd.SampleDesc.Count = MSAASupport;
						break;
					}
					else
					{
						MSAASupport /= 2;
					}
				}
				IDXGIDevice* dxgiDevice = 0;
				hr = GP::GetDevice()->QueryInterface(__uuidof(IDXGIDevice),
					(void**)&dxgiDevice);
				IDXGIAdapter* dxgiAdapter = 0;
				hr = dxgiDevice->GetParent(__uuidof(IDXGIAdapter),
					(void**)&dxgiAdapter);
	
				IDXGIFactory* dxgiFactory = 0;
				hr = dxgiAdapter->GetParent(__uuidof(IDXGIFactory),
						(void**) &dxgiFactory);
				hr = dxgiFactory->CreateSwapChain(GP::GetDevice(), &sd, &pSwapChain);
				
				if (FAILED(hr))
				{
					Log::Get()->Err("Can't create swapchaing");
				}

				RELEASE(dxgiDevice);
				RELEASE(dxgiAdapter);
				RELEASE(dxgiFactory);

				break;
			}

	//		hr = D3D11CreateDeviceAndSwapChain(NULL, driverType, NULL,
	//			NULL, featureLevels, numFeatureLevels, D3D11_SDK_VERSION,
	//			&sd, &pSwapChain, &GP::GetDevice(), &featureLevel, &GP::GetDeviceContext());
	//		
	//		if (SUCCEEDED(hr)) {
	//			break;
	//		}
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

		DepthStencil::InitStencils();

		dsv = new DepthStencil();
		dsv->InitView(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT);

		mainRTV = new RenderTarget(pBackBuffer);
		sceneRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
		diffuseRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
		normalsRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT);
		roughnessRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R32_FLOAT);

		vp.Width = BJEUtils::GetWindowWidth();
		vp.Height = BJEUtils::GetWindowHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;

		svp.Width = RENDERTARGET_WIDTH;
		svp.Height = RENDERTARGET_HEIGHT;
		svp.MinDepth = 0.0f;
		svp.MaxDepth = 1.0f;
		svp.TopLeftX = 0;
		svp.TopLeftY = 0;

		GP::InitShaders();

		return true;
	}

	void Render::BeginFrame()
	{
		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		mainRTV->ClearRTV();
		GP::GetDeviceContext()->OMSetRenderTargets(1, &mainRTV->GetRTV(), NULL);

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

		Blend* blend = new Blend();

		
		cams.push_back(new MainCamera());
		if (!UI::Init(hwnd, cams, this))
		{
			Log::Get()->Err("Incorrect ImGui init!");
		}
		else
		{
			Log::Get()->Debug("ImGui was inited!");
		}

		if (lmananger == nullptr)
		{
			lmananger = new LightMananger();
			lmananger->Init();
		}
		
		mainRTVBuffer = Helper::InitConstantBuffer<BJEStruct::MainSceneProcessingBuffer>(GP::GetDevice());
		mainDesc.gamma = 1.0f;
		mainDesc.expourse = 1.0f;
		mainDesc.itens = 1.0f;

		return true;
	}

	bool Render::DrawWnd()
	{
		if (GetIsResizedState())
		{
			ResizeWindow();
		}

		for (int index = 0; index < shadow.size(); index++)
		{
			if (!lmananger->GetDesc(index).enabled)
				continue;

			shadow[index]->GenerateView(lmananger->GetDesc(index));
			shadow[index]->Draw();

			for (auto& el : elements)
			{
				el->DrawShadow();
			}
		}
		
		// START
		{ 
			BeginFrame();
			UnpackProject();
			cams[0]->DrawCameraObject();
			for (auto& tSound : sound)
			{
				tSound->Play();
			}
		}
		
		//RENDER TO TEXTURES
		{ 
			GP::GetDeviceContext()->RSSetViewports(1, &svp);
			dsv->ClearDepthStencilView();
			sceneRTV->ClearRTV();
			diffuseRTV->ClearRTV();
			normalsRTV->ClearRTV();
			roughnessRTV->ClearRTV();

			ID3D11RenderTargetView* items[4] =
			{
				sceneRTV->GetRTV(),
				diffuseRTV->GetRTV(),
				normalsRTV->GetRTV(),
				roughnessRTV->GetRTV(),
			};

			GP::GetDeviceContext()->OMSetRenderTargets(ARRAYSIZE(items), items, dsv->GetDepthStencil());

			GP::GetDeviceContext()->PSSetSamplers(0, 1, Textures::GetWrapState());

			BaseElement::BindConstantBuffer();
			Materials::BindConstantBuffer();
			GP::BindShader(GP::MODEL_SHADER);

			DrawScene();
			
			
		}
	
		//RENDER TEXTURES ON THE SCENE
		{
			sceneRTV->CreateCopyTexture();
			sceneRTV->ClearRTV();
			GP::GetDeviceContext()->OMSetRenderTargets(1, &sceneRTV->GetRTV(), dsv->GetDepthStencil());

			////////////////// SET TEXTURES
			GP::GetDeviceContext()->PSSetSamplers(0, 1, Textures::GetWrapState());

			GP::GetDeviceContext()->PSSetShaderResources(0, 1, &sceneRTV->GetCopyTexture());
			GP::GetDeviceContext()->PSSetShaderResources(1, 1, &diffuseRTV->GetSRV());
			GP::GetDeviceContext()->PSSetShaderResources(2, 1, &normalsRTV->GetSRV());
			GP::GetDeviceContext()->PSSetShaderResources(3, 1, &roughnessRTV->GetSRV());

			for (int index = 0; index < shadow.size(); index++)
			{
						shadow[index]->BindSRV(index);
			}
			GP::GetDeviceContext()->PSSetSamplers(SHADOW_SAMPLERSTATE_POS, 1, Textures::GetClampState());
			////////////////////////////////
			if (lmananger->IsInited())
				lmananger->Draw();

			///////////////////////////////
			GP::BindShader(GP::DEFFEREDSCENE_SHADER_PS);
			
			Camera::SetCameraBuffer(cams[UI::FocusedCamera()]->GetDesc());

			Blend::Get()->DrawNoBlend();

			DepthStencil::SetDepthStencilState(READ);

			//////////////////////////////
			sceneRTV->DrawTexture();

			

			DepthStencil::SetDepthStencilState(NON);
		}
		
		// SKYBOX
		if (skyBox && skyBox->IsInited())
		{
			skyBox->Draw(cams[UI::FocusedCamera()]->GetDesc());
		}

		GP::GetDeviceContext()->PSSetConstantBuffers(0, 1, &mainRTVBuffer);
		GP::GetDeviceContext()->UpdateSubresource(mainRTVBuffer, 0, NULL, &mainDesc, 0, 0);

		//POST PROCESSING SCENE TEXTURE
		{ 
			for (auto& el : UI::GetPostProcessingStatus())
			{
				if (el.second)
				{
					sceneRTV->CreateCopyTexture();
					sceneRTV->ClearRTV();
					GP::GetDeviceContext()->OMSetRenderTargets(1, &sceneRTV->GetRTV(), NULL);
					sceneRTV->DrawTexture(sceneRTV->GetCopyTexture(), el.first);
				}

			}
		}
		
		//RENDER SCENE TEXTURE ON THE SCREEN
		{ 
			GP::GetDeviceContext()->RSSetViewports(1, &vp);
			GP::GetDeviceContext()->OMSetRenderTargets(1, &mainRTV->GetRTV(), NULL);

			
			
			mainRTV->DrawTexture(sceneRTV->GetSRV(), SCENE);
		}

		// END
		EndFrame();

		if (PackMananger::Get()->GetSavingStatus())
		{
			for (auto& el : objects)
				el->Draw(cams[UI::FocusedCamera()]->GetDesc());

			PackMananger::Get()->Close();
		}
		if (UnpackMananger::Get()->GetOpeningStatus())
		{
			UnpackMananger::Get()->Reset();
		}

		return true;
	}
	
	bool Render::DrawScene()
	{

		for (auto& el : elements)
		{
			el->Draw(cams[UI::FocusedCamera()]->GetDesc());
		}


		
		return true;
	}

	Object* Render::InitObjs(Object* object)
	{

		if (!object->IsInited())
		{
			if (!object->Init()) {
				return nullptr;
			};
		}
		std::vector<Element*> tmp = std::move(object->MoveElements());
		elements.insert(elements.end(), tmp.begin(), tmp.end());

		std::sort(elements.begin(), elements.end(), [](BaseElement*& a, BaseElement*& b) { return a < b; });
		
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
			if (!skyBox->Init())
			{
				Log::Get()->Err("SkyBox didn't inited!");
			}
		}

	}

	void Render::ResizeWindow()
	{
		GP::GetDeviceContext()->OMSetRenderTargets(0, 0, 0);

		CLOSE(mainRTV);
		
		HRESULT hr = S_OK;

		hr = pSwapChain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
		
		ID3D11Texture2D* pBuffer;
		hr = pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
			(void**)&pBuffer);
		
		mainRTV = new RenderTarget(pBuffer);



		vp.Width = GetWindowWidth();
		vp.Height = GetWindowHeight();


		GP::GetDeviceContext()->RSSetViewports(1, &vp);
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
			LightDesc ld;
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
		if (lmananger->AddLight(ld) && ld.shadowEnabled && shadow.size() <= MAX_SHADOW_NUM)
		{	
			if(ld.lightType == 1)
				shadow.push_back(new OmnidirectionalShadow());
			if (ld.lightType == 0)
				shadow.push_back(new SimpleShadow());
		};

		islight = true;
	}

	void Render::Close()
	{
		for (auto& el : elements) {
			CLOSE(el);
		}
		for (auto& el : objects) {
			CLOSE(el);
		}
		objects.clear();
		
		for (auto& el : shadow)
		{
			CLOSE(el);
		}

		for (auto& el : cams)
		{
			CLOSE(el);
		}
		cams.clear();

		
		GP::ClearGlobalParameters();

		RELEASE(pSwapChain)
			;
		CLOSE(normalsRTV);
		CLOSE(diffuseRTV);
		CLOSE(sceneRTV);
		CLOSE(mainRTV);
		CLOSE(skyBox);

		Blend::Get()->Close();
		Log::Get()->Debug("Render was closed");
		PackMananger::Get()->Close();
		UnpackMananger::Get()->Close();
		CLOSE(lmananger);
	}


}