#include "LightMananger.h"

namespace BJEngine
{
	ID3D11Buffer* LightMananger::lightBuffer = nullptr;
	LightMananger* LightMananger::instance = nullptr;
	bool LightMananger::Init()
	{
		
		if (!isInited)
		{
			if (instance == nullptr)
				instance = this;

			lightBuffer = Helper::InitConstantBuffer<ConstantBufferLight>(GP::GetDevice());
			if (lightBuffer == nullptr) {
				Log::Get()->Err("light create error");
				return FAILED(E_FAIL);
			}
			isInited = true;
		}
		Log::Get()->Debug("LightMananger inited!");
		return true;
	}

	bool LightMananger::AddLight(LightDesc& desc)
	{


		if (lDesc.lightsCount == MAX_LIGHT_NUM)
		{
			return false;
		}
		lDesc.light[lDesc.lightsCount] = desc;
		


		lDesc.lightsCount++;
		Log::Get()->Debug("Created light!");


		return true;
	}
	
	bool LightMananger::Draw()
	{
		GP::GetDeviceContext()->UpdateSubresource(lightBuffer, 0, NULL, &lDesc, 0, 0);
		GP::GetDeviceContext()->PSSetConstantBuffers(0, 1, &lightBuffer); 

		if (PackMananger::Get()->GetSavingStatus())
		{
			PackLights();
		}

		return true;
	}

	const bool LightMananger::IsInited()
	{
		return isInited;
	}

	const bool LightMananger::IsHaveLights()
	{
		return lDesc.lightsCount;
	}
	
	void LightMananger::Close()
	{
		RELEASE(lightBuffer);
	}

	bool LightMananger::PackLights()
	{
		for (size_t index = 0; index < lDesc.lightsCount; index++)
		{
			LightType ltPack;

			ltPack.angle = lDesc.light[index].angle;
			ltPack.att = lDesc.light[index].att;
			ltPack.color = lDesc.light[index].color;
			ltPack.dir = lDesc.light[index].dir;
			ltPack.enabled = lDesc.light[index].enabled;
			ltPack.lightType = lDesc.light[index].lightType;
			ltPack.pos = lDesc.light[index].pos;
			
			PackMananger::Get()->AddLight(ltPack);
		}


		return true;
	}


}