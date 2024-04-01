#include "LightMananger.h"

namespace BJEngine
{
	ID3D11Buffer* LightMananger::lightBuffer = nullptr;
	ID3D11Buffer* LightMananger::lightBuffer2 = nullptr;
	ID3D11ShaderResourceView* LightMananger::srvv = nullptr;

	bool LightMananger::Init()
	{
		
		if (!isInited)
		{
			lightBuffer = Helper::InitConstantBuffer<BJEStruct::ConstantBufferLight>(GP::GetDevice());
			if (lightBuffer == nullptr) {
				Log::Get()->Err("light create error");
				return FAILED(E_FAIL);
			}

			
			isInited = true;
		}

		

		Log::Get()->Debug("LightMananger inited!");
		return true;
	}

	bool LightMananger::AddLight(BJEStruct::LightDesc& desc)
	{
		lights.push_back(desc);

		if (dynamicMaxLightSize <= lights.size())
		{
			dynamicMaxLightSize += lights.size();
			isInitedBuffer = false;
		}
		
		if (!isInitedBuffer)
		{
			isInitedBuffer = InitStructuredBuffer();
		}

		Log::Get()->Debug("Create light!");
		return true;
	}
	
	bool LightMananger::Draw()
	{
		BJEStruct::ConstantBufferLight lDesc;

		lDesc.lightsCount = lights.size();
		lDesc.enablePBR = pbrEnable;
		lDesc.enableIBR = ibrEnable;

		GP::GetDeviceContext()->UpdateSubresource(lightBuffer, 0, NULL, &lDesc, 0, 0);
		GP::GetDeviceContext()->PSSetConstantBuffers(0, 1, &lightBuffer); 

		if (lightBuffer2 != nullptr)
		{
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			ZeroMemory(&mappedResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
			HRESULT hr = GP::GetDeviceContext()->Map(lightBuffer2, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
			memcpy(mappedResource.pData, &lights[0], sizeof(BJEStruct::LightDesc) * lights.size());
			GP::GetDeviceContext()->Unmap(lightBuffer2, 0);
			GP::GetDeviceContext()->PSSetShaderResources(15, 1, &srvv);
		}
		else if (lights.size() != 0)
		{

		}

		return true;
	}

	const bool LightMananger::IsInited()
	{
		return isInited;
	}

	const bool LightMananger::IsHaveLights()
	{
		return lights.size();
	}
	
	void LightMananger::Close()
	{
		RELEASE(lightBuffer);
		RELEASE(lightBuffer2);
		RELEASE(srvv);
	}
	bool LightMananger::InitStructuredBuffer()
	{
		RELEASE(lightBuffer2);
		RELEASE(srvv);

		D3D11_BUFFER_DESC sbDesc;
		sbDesc.ByteWidth = sizeof(BJEStruct::LightDesc) * dynamicMaxLightSize;
		sbDesc.Usage = D3D11_USAGE_DYNAMIC;
		sbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		sbDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		sbDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		sbDesc.StructureByteStride = sizeof(BJEStruct::LightDesc);

		D3D11_SUBRESOURCE_DATA subResourceData2;
		subResourceData2.pSysMem = &lights[0];

		HRESULT result = GP::GetDevice()->CreateBuffer(&sbDesc, &subResourceData2, &lightBuffer2);

		if (FAILED(result))
		{
			Log::Get()->Err(std::system_category().message(result).c_str());
		}

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFER;
		srvDesc.Buffer.FirstElement = 0;
		srvDesc.Buffer.NumElements = dynamicMaxLightSize;

		result = GP::GetDevice()->CreateShaderResourceView(lightBuffer2, &srvDesc, &srvv);
		if (FAILED(result))
		{
			Log::Get()->Err(std::system_category().message(result).c_str());
		}

		return true;
	}
}