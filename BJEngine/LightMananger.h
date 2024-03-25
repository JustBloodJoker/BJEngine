#pragma once 
#include "pch.h"
#include "Light.h"
#include "Object.h"


namespace BJEngine
{ 
	
	static class LightMananger
	{
		friend class UI;

		static ID3D11Buffer* lightBuffer;
		static ID3D11Buffer* lightBuffer2;
		static ID3D11ShaderResourceView* srvv;

	public:

		bool Init();
		bool AddLight(BJEStruct::LightDesc& desc);
		bool Draw();

		const bool IsInited();
		const bool IsHaveLights();

		void Close();

		int GetType(int index)
		{
			return lights[index].lightType;
		}
		BJEStruct::LightDesc GetDesc(int index)
		{
			return lights[index];
		}
		const int GetCurrentCount()
		{
			return lights.size();
		}
		std::vector<BJEStruct::LightDesc>& GetLights()
		{
			return lights;
		}

	private:

		bool InitStructuredBuffer();

		bool isInited = false;
		bool isInitedBuffer = false;

		std::vector<BJEStruct::LightDesc> lights;
		int dynamicMaxLightSize = 5;
		bool pbrEnable = false;
		bool ibrEnable = false;
	};


}