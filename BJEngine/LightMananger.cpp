#include "LightMananger.h"

namespace BJEngine
{
	bool LightMananger::isInited = false;

	ID3D11Buffer* LightMananger::lightBuffer = nullptr;
	LightMananger::ConstantBufferLight LightMananger::lightDescBuffer;
	Shadow* LightMananger::shadows[MAX_LIGHT_NUM] = { nullptr };

	ID3D11Device* LightMananger::pd3dDevice = nullptr;

	bool LightMananger::Init(ID3D11Device* pd3dDevice)
	{
		
		if (!isInited)
		{
			LightMananger::pd3dDevice = pd3dDevice;
			lightBuffer = Helper::InitConstantBuffer<ConstantBufferLight>(pd3dDevice);
			if (lightBuffer == nullptr) {
				Log::Get()->Err("light create error");
				return FAILED(E_FAIL);
			}
			isInited = true;
		}
		Log::Get()->Debug("LightMananger inited!");
		return true;
	}

	bool LightMananger::AddLight(LightDesc desc)
	{


		if (lightDescBuffer.lightsCount == MAX_LIGHT_NUM)
		{
			return false;
		}
		lightDescBuffer.light[lightDescBuffer.lightsCount] = desc;
		shadows[lightDescBuffer.lightsCount] = new Shadow();
		shadows[lightDescBuffer.lightsCount]->InitShadow(pd3dDevice, desc.lightType);


		lightDescBuffer.lightsCount++;
		Log::Get()->Debug("Created light!");


		return true;
	}
	
	bool LightMananger::Draw(ID3D11DeviceContext* pImmediateContext)
	{
		ImGui::Begin("ddd");

		static bool fd = true;
		ImGui::Checkbox("ddd", &fd);
		lightDescBuffer.pad = fd;

		ImGui::End();

		pImmediateContext->UpdateSubresource(lightBuffer, 0, NULL, &lightDescBuffer, 0, 0);
		pImmediateContext->PSSetConstantBuffers(0, 1, &lightBuffer); 

		for (int index = 0; index < lightDescBuffer.lightsCount; index++)
		{
			if (LightMananger::GetType(index) == BJEUtils::POINTLIGHT)
				pImmediateContext->PSSetShaderResources(5 + index, 1, shadows[index]->GetTexture());
			else
				pImmediateContext->PSSetShaderResources(5 + index + MAX_LIGHT_NUM, 1, shadows[index]->GetTexture());
		}

		if (PackMananger::Get()->GetSavingStatus())
		{
			PackLights();
		}

		return true;
	}

	bool LightMananger::DrawShadows(ID3D11DeviceContext* pImmediateContext, std::vector<Object*> objects)
	{

		for (size_t index = 0; index < lightDescBuffer.lightsCount; index++)
		{
			shadows[index]->Render(pImmediateContext, LightMananger::GetDesc(index), objects);
		}


		return true;
	}

	bool LightMananger::SetMatrix(Object* object)
	{
		for (int index = 0; index < lightDescBuffer.lightsCount; index++)
		{
			object->SetLightViewAndProjectionMatrix(shadows[index]->GetView(), shadows[index]->GetProjection(), index);
		}

		return true;
	}

	const bool LightMananger::IsInited()
	{
		return isInited;
	}

	const bool LightMananger::IsHaveLights()
	{
		return lightDescBuffer.lightsCount;
	}
	
	void LightMananger::Close()
	{
		RELEASE(lightBuffer);
	}

	bool LightMananger::PackLights()
	{
		for (size_t index = 0; index < lightDescBuffer.lightsCount; index++)
		{
			LightType ltPack;

			ltPack.angle = lightDescBuffer.light[index].angle;
			ltPack.att = lightDescBuffer.light[index].att;
			ltPack.color = lightDescBuffer.light[index].color;
			ltPack.dir = lightDescBuffer.light[index].dir;
			ltPack.enabled = lightDescBuffer.light[index].enabled;
			ltPack.lightType = lightDescBuffer.light[index].lightType;
			ltPack.pos = lightDescBuffer.light[index].pos;

			PackMananger::Get()->AddLight(ltPack);
		}


		return true;
	}


}