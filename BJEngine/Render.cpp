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
		sceneRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT,true, false);
		diffuseRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, false, false);
		normalsRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, false, false);
		specularRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R16G16B16A16_FLOAT, false, false);
		roughnessRTV = new RenderTarget(RENDERTARGET_WIDTH, RENDERTARGET_HEIGHT, DXGI_FORMAT_R32_FLOAT, false, false);


		irradianceRTV = new RenderTarget(SHADOW_WIDTH, SHADOW_WIDTH, DXGI_FORMAT_R32G32B32A32_FLOAT, false, 6, true, false);

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
			UI::SetLightMananger(lmananger);
		}
		
		mainRTVBuffer = Helper::InitConstantBuffer<BJEStruct::MainSceneProcessingBuffer>(GP::GetDevice());
		mainDesc.gamma = 1.0f;
		mainDesc.expourse = 1.0f;
		mainDesc.itens = 1.0f;
		
		elementSkyBoxConvertion = new ElementSkyBoxConvertion();
		elementSkyBoxConvertion->Init();

		return true;
	}

	bool Render::DrawWnd()
	{
		if (GetIsResizedState())
		{
			ResizeWindow();
		}

		for (int index = 0, lightindex = 0; index < shadow.size(); index++, lightindex++)
		{
			if (!lmananger->GetDesc(lightindex).shadowEnabled)
			{
				index--;
				continue;
			}

			if (!lmananger->GetDesc(lightindex).enabled)
			{
				continue;
			}
			
			
			shadow[index]->GenerateView(lmananger->GetDesc(lightindex));
			shadow[index]->Draw();

			for (auto& el : defferedElements)
			{
				el->DrawShadow(GP::GetDeviceContext());
			}
		}
		
		if (!elementSkyBoxConvertion->IsDraw() && forwardElements.size() != 0 && dynamic_cast<ElementSkyBox*>(*(forwardElements.end() - 1)) != nullptr )
		{
			irradianceRTV->ClearRTV();
			GP::GetDeviceContext()->OMSetRenderTargets(1, &irradianceRTV->GetRTV(), NULL);
			GP::GetDeviceContext()->PSSetShaderResources(0, 1, &dynamic_cast<ElementSkyBox*>(*(forwardElements.end() - 1))->GetTexture()->GetTexture());
			GP::GetDeviceContext()->PSSetSamplers(0, 1, Textures::GetWrapState());
			elementSkyBoxConvertion->Draw(cams[UI::FocusedCamera()]->GetDesc(), GP::GetDeviceContext());
			(*(forwardElements.end() - 1))->DrawShadow(GP::GetDeviceContext());
		}

		// START
		{ 
			BeginFrame();

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
			specularRTV->ClearRTV();
			roughnessRTV->ClearRTV();

			ID3D11RenderTargetView* items[5] =
			{
				sceneRTV->GetRTV(),
				diffuseRTV->GetRTV(),
				normalsRTV->GetRTV(),
				specularRTV->GetRTV(),
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

			GP::GetDeviceContext()->PSSetShaderResources(WORLDPOS_DEFPASS_TEXTURE_POS, 1, &sceneRTV->GetCopyTexture());
			GP::GetDeviceContext()->PSSetShaderResources(DIFFUSE_DEFPASS_TEXTURE_POS, 1, &diffuseRTV->GetSRV());
			GP::GetDeviceContext()->PSSetShaderResources(NORMAL_DEFPASS_TEXTURE_POS, 1, &normalsRTV->GetSRV());
			GP::GetDeviceContext()->PSSetShaderResources(SPECULAR_DEFPASS_TEXTURE_POS, 1, &specularRTV->GetSRV());
			GP::GetDeviceContext()->PSSetShaderResources(ROUGHNESS_DEFPASS_TEXTURE_POS, 1, &roughnessRTV->GetSRV());
			//GP::GetDeviceContext()->PSSetShaderResources(EMISSION_DEFPASS_TEXTURE_POS, 1, &);

			GP::GetDeviceContext()->PSSetShaderResources(IRRADIANCE_DEFPASS_TEXTURE_POS, 1, &irradianceRTV->GetSRV());


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

			

			GP::ResetShaders();
		}
		
		for (auto& el : forwardElements)
			el->Draw(cams[UI::FocusedCamera()]->GetDesc(), GP::GetDeviceContext());

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

		ExecuteFileSystemCommand();

		return true;
	}
	

	bool Render::DrawScene()
	{

		for (auto& el : defferedElements)
		{
			el->Draw(cams[UI::FocusedCamera()]->GetDesc(), GP::GetDeviceContext());
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
		std::vector<BaseElement*> tmp = std::move(object->MoveElements());
		defferedElements.insert(defferedElements.end(), tmp.begin(), tmp.end());

		std::sort(defferedElements.begin(), defferedElements.end(), [](BaseElement*& a, BaseElement*& b) { return a < b; });
		
		objects.push_back(object);

		return object;
	}

	void Render::AddSkyBox(std::string texturePath)
	{
		ElementSkyBox* ptr = nullptr;
		if (!forwardElements.empty())
		{
			ptr = dynamic_cast<ElementSkyBox*>(*(forwardElements.end() - 1));
			if (ptr)
			{
				CLOSE(ptr);
				forwardElements.pop_back();
			}
		}
		ptr = new ElementSkyBox(new Textures(std::wstring(texturePath.begin(), texturePath.end())));

		if (!ptr)
		{
			Log::Get()->Err("SkyBox didn't inited!");
			return;
		}
		forwardElements.push_back(ptr);
		elementSkyBoxConvertion->Redraw();
		
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

	void Render::SetLight(BJEStruct::LightDesc ld)
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

	void Render::LoadProject(std::string path)
	{
		fileSysExecution.push(new FileOpen(std::move(path), &defferedElements, &forwardElements, &ldtmp));
	}

	void Render::SaveProject(std::string path, std::string name)
	{
		fileSysExecution.push(new FileSave(std::move(path), std::move(name), lmananger->GetLights(), Materials::GetAllMaterials(), defferedElements, forwardElements));
	}

	void Render::ExecuteFileSystemCommand()
	{
		while (!fileSysExecution.empty())
		{
			fileSysExecution.front()->Execute();
			auto ptr = dynamic_cast<FileOpen*>(fileSysExecution.front());
			if (ptr)
			{
				for (auto& el : ldtmp)
				{
					SetLight(el);
				}
			}
			fileSysExecution.pop();
		}
	}

	void Render::Close()
	{
		for (auto& el : defferedElements) {
			CLOSE(el);
		}
		for (auto& el : forwardElements) {
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


		Blend::Get()->Close();
		Log::Get()->Debug("Render was closed");
		CLOSE(lmananger);
	}


}