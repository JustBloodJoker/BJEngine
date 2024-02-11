#include "Render.h"
#include "UI.h"
#include "LightMananger.h"
#include "Model.h"


namespace BJEngine {


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
				&sd, &pSwapChain, &GP::GetDevice(), &featureLevel, &GP::GetDeviceContext());

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

		DepthStencil::InitStencils();

		mainRTV = new RenderTarget(pBackBuffer);
		sceneRTV = new RenderTarget(1920, 1080);
		
		vp.Width = BJEUtils::GetWindowWidth();
		vp.Height = BJEUtils::GetWindowHeight();
		vp.MinDepth = 0.0f;
		vp.MaxDepth = 1.0f;
		vp.TopLeftX = 0;
		vp.TopLeftY = 0;


		svp.Width = 1920;
		svp.Height = 1080;
		svp.MinDepth = 0.0f;
		svp.MaxDepth = 1.0f;
		svp.TopLeftX = 0;
		svp.TopLeftY = 0;

		GP::InitShaders();;

		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		return true;
	}

	void Render::BeginFrame()
	{
		float ClearColor[4] = { 0.1f, 0.1f, 0.1f, 1.0f };
		GP::GetDeviceContext()->RSSetViewports(1, &vp);

		mainRTV->ClearRTV();
		mainRTV->Bind(NULL);

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

		if (!LightMananger::IsInited())
		{
			LightMananger::Init();
		}
		
		Materials::SetObjectPtr(&objects);

		return true;
	}

	bool Render::DrawWnd()
	{
		

		if (GetIsResizedState())
		{
			ResizeWindow();
		}

		LightMananger::DrawShadows(objects);

		BeginFrame();

		UnpackProject();

		cams[0]->DrawCameraObject();

		for (auto& tSound : sound)
		{
			tSound->Play();
		}
		
		GP::GetDeviceContext()->RSSetViewports(1, &svp);
		sceneRTV->ClearDSV();
		sceneRTV->ClearRTV();
		sceneRTV->Bind();

		DrawScene();

		

		for (auto& el : UI::GetPostProcessingStatus())
		{
			if (el.second)
			{
				sceneRTV->CreateCopyTexture();
				sceneRTV->ClearRTV();
				sceneRTV->Bind(NULL);
				sceneRTV->DrawTexture(sceneRTV->GetCopyTexture(), el.first);
			}

		}




		GP::GetDeviceContext()->RSSetViewports(1, &vp);
		mainRTV->Bind(NULL);
		mainRTV->DrawTexture(sceneRTV->GetSRV(), SCENE);

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

	bool Render::DrawScene()
	{
		//for (auto& el : cams)
		//{
		//	el->DrawCameraObject(cams[UI::FocusedCamera()]->GetDesc().viewMatrix, cams[UI::FocusedCamera()]->GetDesc().projectionMatrix);
		//}

		if (LightMananger::IsInited())
			LightMananger::Draw();
		
		objects.push_back(skyBox);

		GP::GetDeviceContext()->PSSetSamplers(0, 1, Textures::GetBorderState());

		for (auto& object : objects)
		{
			if (object && object->IsInited())
			{
				LightMananger::SetMatrix(object);
				object->Draw(cams[UI::FocusedCamera()]->GetDesc());
			}
		}

		objects.pop_back();

		return true;
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

		
		GP::ClearGlobalParameters();

		RELEASE(pSwapChain);
		
		CLOSE(mainRTV);
		CLOSE(skyBox);

		Blend::Get()->Close();
		Log::Get()->Debug("Render was closed");
		PackMananger::Get()->Close();
		UnpackMananger::Get()->Close();
		LightMananger::Close();
	}


}