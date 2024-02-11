#pragma once 
#include "pch.h"
#include "Light.h"
#include "Object.h"
#include "Shadow.h"

namespace BJEngine
{

	static class LightMananger
	{
		friend class UI;

	public:

		static bool Init();
		static bool AddLight(LightDesc desc);
		static bool Draw();
		
		//SHADOWS
		static bool DrawShadows(std::vector<Object*> objects);
		static bool SetMatrix(Object* object);

		//SHADOWS

		static const bool IsInited();
		static const bool IsHaveLights();

		static void Close();

		//////////  Œ—“€À‹ ≈¡¿Õ€…
		
		static int GetType(int index)
		{
			return lightDescBuffer.light[index].lightType;
		}
		static LightDesc GetDesc(int index)
		{
			return lightDescBuffer.light[index];
		}

		//////////  Œ—“€À‹ ≈¡¿Õ€…

	private:

		static Shadow* shadows[MAX_LIGHT_NUM];
		
		static bool PackLights();

		static bool isInited;

		struct ConstantBufferLight
		{
			LightDesc light[MAX_LIGHT_NUM];
			int lightsCount = 0;
			float gamma = 1.0f;
			int pad1;
			int pad2;
		};

		static ID3D11Buffer* lightBuffer;
		static ConstantBufferLight lightDescBuffer;

	};


}