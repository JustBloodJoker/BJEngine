#include "RenderTarget.h"


namespace BJEngine
{
	unsigned int RenderTarget::size = 0;;
	
	RenderTarget::RenderTarget(ID3D11Texture2D* texture, bool MSAA)
	{
		size++;

		renderTargetTexture = texture;
		D3D11_TEXTURE2D_DESC DS;
		texture->GetDesc(&DS);


		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = DS.Format;
		if (MSAA)
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
		}
		HRESULT hr = GP::GetDevice()->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &renderTargetView);
		if (FAILED(hr)) {
			Log::Get()->Err("RTV create error ", size);
			size--;
		}

		Screen::Init();
	}

	RenderTarget::RenderTarget(int width, int height, bool MSAA)
	{
		size++;

		D3D11_TEXTURE2D_DESC textureDesc;
		ZeroMemory(&textureDesc, sizeof(textureDesc));
		textureDesc.Width = width;
		textureDesc.Height = height;
		textureDesc.MipLevels = 1;
		textureDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
		
		textureDesc.ArraySize = 1; 
		textureDesc.MiscFlags = 0;
		
		textureDesc.SampleDesc.Count = 1;
		textureDesc.SampleDesc.Quality = 0;

		textureDesc.Usage = D3D11_USAGE_DEFAULT;
		textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
		textureDesc.CPUAccessFlags = 0;
		

		HRESULT result = GP::GetDevice()->CreateTexture2D(&textureDesc, NULL, &renderTargetTexture);
		result = GP::GetDevice()->CreateTexture2D(&textureDesc, NULL, &tempRenderTargetTexture);
		if (FAILED(result))
		{
			Log::Get()->Err("Can't create texture for RTV ", size);
			size--;
			return;
		}

		D3D11_RENDER_TARGET_VIEW_DESC renderTargetViewDesc;
		renderTargetViewDesc.Format = textureDesc.Format;
		if (MSAA)
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			renderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
			renderTargetViewDesc.Texture2D.MipSlice = 0;
		}
		result = GP::GetDevice()->CreateRenderTargetView(renderTargetTexture, &renderTargetViewDesc, &renderTargetView);
		

		
		if (FAILED(result))
		{
			Log::Get()->Err("Can't create RTV ", size);
			size--;
			return;
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC shaderResourceViewDesc;
		shaderResourceViewDesc.Format = textureDesc.Format;
		if (MSAA)
		{
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
		}
		else
		{
			shaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
			shaderResourceViewDesc.Texture2D.MipLevels = 1;
			shaderResourceViewDesc.Texture2D.MostDetailedMip = 0;
		}

		result = GP::GetDevice()->CreateShaderResourceView(renderTargetTexture, &shaderResourceViewDesc, &shaderResourceView);
		result = GP::GetDevice()->CreateShaderResourceView(tempRenderTargetTexture, &shaderResourceViewDesc, &tempShaderResourceView);

		if (FAILED(result))
		{
			Log::Get()->Err("Can't create shaderResourceView for RTV ", size);
			size--;
			return;
		}

		
	
	}

	ID3D11RenderTargetView*& RenderTarget::GetRTV()
	{
		return renderTargetView;
	}

	void RenderTarget::ClearRTV()
	{
		
		GP::GetDeviceContext()->ClearRenderTargetView(renderTargetView, clearColor);
	}

	void RenderTarget::CreateCopyTexture()
	{
		GP::GetDeviceContext()->CopyResource(tempRenderTargetTexture, renderTargetTexture);
	}

	ID3D11ShaderResourceView* RenderTarget::GetCopyTexture()
	{
		return tempShaderResourceView;
	}

	void RenderTarget::Close()
	{
		RELEASE(tempRenderTargetTexture);
		RELEASE(renderTargetView);
		RELEASE(renderTargetTexture);
		RELEASE(shaderResourceView);
				
		size--;

		if (size == 0)
			Screen::Close();
	}

	ID3D11ShaderResourceView* RenderTarget::GetSRV()
	{
		return shaderResourceView;
	}

	ID3D11Texture2D* RenderTarget::GetTexture()
	{
		return renderTargetTexture;
	}

	void RenderTarget::SaveRTVTexture(std::wstring&& fileName)
	{
		D3DX11SaveTextureToFile(GP::GetDeviceContext(), renderTargetTexture, D3DX11_IFF_PNG, fileName.c_str());
	}

	void RenderTarget::DrawTexture(ID3D11ShaderResourceView* srv, BJEUtils::POST_PROCESSING type)
	{
		Screen::Draw(srv, type);
	}

	BJEStruct::VertexBackGround RenderTarget::Screen::vertices[4] = 
	{ 
		BJEStruct::VertexBackGround(-1.0f, 1.0f, 0.0f, 0.0f, 0.0f),
		BJEStruct::VertexBackGround(1.0f, 1.0f, 0.0f,  1.0f, 0.0f),
		BJEStruct::VertexBackGround(-1.0f, -1.0f, 0.0f,0.0f, 1.0f),
		BJEStruct::VertexBackGround(1.0f, -1.0f, 0.0f, 1.0f, 1.0f)
	};
	
	ID3D11Buffer* RenderTarget::Screen::pVertexBuffer = nullptr;
	Shader* RenderTarget::Screen::shader = nullptr;
	Shader* RenderTarget::Screen::inverseShader = nullptr;
	Shader* RenderTarget::Screen::greyShader = nullptr;
	Shader* RenderTarget::Screen::sharpnessShader = nullptr;
	Shader* RenderTarget::Screen::simpleBlurShader = nullptr;
	Shader* RenderTarget::Screen::boundaryDelineationShader = nullptr;


	bool RenderTarget::Screen::isInited = false;

	void RenderTarget::Screen::Init()
	{
		if (!isInited)
		{
			HRESULT hr = S_OK;

			D3D11_INPUT_ELEMENT_DESC layout[] =
			{
				{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT,  0, 0,  D3D11_INPUT_PER_VERTEX_DATA, 0 },
				{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,     0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 }
			};

			shader = new BJEngine::Shader(MAIN_RTV_SHADER, RTV_VS_SHADER, "VS", "PS");

			shader->SetInputLayout(layout, ARRAYSIZE(layout));
			shader->Init();

			inverseShader = new Shader(INVERSE_RTV_SHADER, L"", "", "PS");
			inverseShader->Init();

			greyShader = new Shader(GREY_RTV_SHADER, L"", "", "PS");
			greyShader->Init();

			sharpnessShader = new Shader(SHARPNESS_RTV_SHADER, L"", "", "PS");
			sharpnessShader->Init();

			simpleBlurShader = new Shader(SIMPLE_BLUR_RTV_SHADER, L"", "", "PS");
			simpleBlurShader->Init();

			boundaryDelineationShader = new Shader(BOUNDARY_DELINEATION_RTV_SHADER, L"", "", "PS");
			boundaryDelineationShader->Init();

			pVertexBuffer = Helper::InitVertexBuffer(GP::GetDevice(), sizeof(BJEStruct::VertexBackGround) * 4, &vertices[0]);
			isInited = true;
		}
	}

	void RenderTarget::Screen::Draw(ID3D11ShaderResourceView* srv, BJEUtils::POST_PROCESSING type)
	{
		if (shader != nullptr)
		{
			UINT stride = sizeof(BJEStruct::VertexBackGround);
			UINT offset = 0;

			GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
			GP::GetDeviceContext()->IASetInputLayout(shader->GetInputLayout());
			GP::GetDeviceContext()->VSSetShader(shader->GetVertexShader(), NULL, 0);
			GP::GetDeviceContext()->PSSetShader(GetShader(type)->GetPixelShader(), NULL, 0);
			GP::GetDeviceContext()->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

			GP::GetDeviceContext()->PSSetShaderResources(DIFFUSE_TEXTURE_POS, 1, &srv);
			GP::GetDeviceContext()->PSSetSamplers(DIFFUSE_SAMPLERSTATE_POS, 1, Textures::GetWrapState());


			GP::GetDeviceContext()->Draw(4, 0);

			GP::GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		}
	}

	void RenderTarget::Screen::Close()
	{
		RELEASE(pVertexBuffer);
		CLOSE(shader);
		CLOSE(inverseShader);
		CLOSE(greyShader);
		CLOSE(sharpnessShader);
		CLOSE(simpleBlurShader);
		CLOSE(boundaryDelineationShader);
		isInited = false;
	}

	Shader* RenderTarget::Screen::GetShader(BJEUtils::POST_PROCESSING type)
	{
		switch (type)
		{
		case BJEUtils::SCENE:
			return shader;
			break;
		
		case BJEUtils::INVERSION:
			return inverseShader;
			break;
		
		case BJEUtils::GREY:
			return greyShader;
			break;
		
		case BJEUtils::SHARPNESS:
			return sharpnessShader;
			break;
		
		case BJEUtils::SIMPLE_BLUR:
			return simpleBlurShader;
			break;
		
		case BJEUtils::BOUNDARY_DELINEATION:
			return boundaryDelineationShader;
			break;
		}





		return shader;
	}
	
}