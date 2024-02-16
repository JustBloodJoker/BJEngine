#pragma once 
#include "pch.h"
#include "Textures.h"
#include "RenderTarget.h"
#include "Object.h"

namespace BJEngine
{
	enum MATERIAL_TYPE
	{
		AMBIENT,
		DIFFUSE,
		SPECULAR,
		EMISSIVE,
		SPECULAR_POWER,
		HAS_TEXTURE,
		HAS_NORMAL_TEXTURE,
		HAS_ROUGHNESS_TEXTURE,
		HAS_EMISSION_TEXTURE,
		HAS_SPECULAR_TEXTURE,
		HAS_REFLECTION
	};

	class Object;
	class Element;

	class Materials
	{

	
		static ID3D11Buffer* pMaterialBuffer;

	public:

		Materials();

		void SetParam(MATERIAL_TYPE paramType, dx::XMFLOAT4 param);
		void SetParam(MATERIAL_TYPE paramType, float param);

		void SetTexture(MATERIAL_TYPE textureType, std::wstring textureName);
		void SetTexture(MATERIAL_TYPE textureType, Textures* texture);

		void Draw();

		void Close();

		static void BindConstantBuffer()
		{
			if(pMaterialBuffer == nullptr)
				pMaterialBuffer = Helper::InitConstantBuffer<ConstantMaterialBuffer>(GP::GetDevice());

			GP::GetDeviceContext()->PSSetConstantBuffers(2, 1, &pMaterialBuffer);
		}

		int GenRenderPriority();

	private:


		void Init();

		
		Textures* texture;
		Textures* normalTexture;
		Textures* roughnessTexture;
		Textures* emissionTexture;
		Textures* specularTexture;
		
		bool isReflect;

		struct MaterialDesc
		{
			MaterialDesc()
			{
				ZeroMemory(this, sizeof(MaterialDesc));
			}

			dx::XMFLOAT4 emissive;
			dx::XMFLOAT4 ambient;
			dx::XMFLOAT4 diffuse;
			dx::XMFLOAT4 specular;

			float specularPower;

			int isTexture;
			int isNormalTexture;
			int isRoughnessTexture;
			int isEmissionTexture;
			int isSpecularTexture;
			int ishaveTransparency;

			int ishavealphablend;
			
			
		};
		struct ConstantMaterialBuffer
		{
			MaterialDesc matDesc;
		};
		

		

		ConstantMaterialBuffer cmbDesc;

		bool isInit;

		void HasAlphaChannel(ID3D11ShaderResourceView* textureSRV);
	};


}