#pragma once
#include "pch.h"

namespace BJEngine {


	class Light
	{
	private:

		friend class DirectionalLight;
		friend class PointLight;
		friend class SpotLight;

	public:

		Light();
		~Light();

		void Close();

		ID3D11Buffer* GetConstantBuffer();
		bool IsLightOn();

		virtual bool InitLight(ID3D11Device* pd3dDevice);
		virtual void DrawLight(ID3D11DeviceContext* pImmediateContext);

	private:

		ID3D11Buffer* lightBuffer;
		bool isLightOn = false;

	};


}