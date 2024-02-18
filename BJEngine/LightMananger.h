#pragma once 
#include "pch.h"
#include "Light.h"
#include "Object.h"


namespace BJEngine
{

	static class LightMananger
	{
		friend class UI;

		struct ConstantBufferLight
		{
			LightDesc light[MAX_LIGHT_NUM];
			int lightsCount = 0;
			int pad3;
			int pad1;
			int pad2;
		};

		static ID3D11Buffer* lightBuffer;
		
		static LightMananger* instance;
	public:

		bool Init();
		bool AddLight(LightDesc& desc);
		bool Draw();

		const bool IsInited();
		const bool IsHaveLights();

		void Close();

		//////////  Œ—“€À‹ ≈¡¿Õ€…
		
		int GetType(int index)
		{
			return lDesc.light[index].lightType;
		}
		LightDesc GetDesc(int index)
		{
			return lDesc.light[index];
		}
		const int GetCurrentCount()
		{
			return lDesc.lightsCount - 1;
		}
		//////////  Œ—“€À‹ ≈¡¿Õ€…

	private:

		bool PackLights();
		
		bool isInited;

		ConstantBufferLight lDesc;		
	};


}