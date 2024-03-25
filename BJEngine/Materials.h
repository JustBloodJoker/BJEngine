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
		HAS_REFLECTION,
		OPACITY,

	};

	

	class Object;
	class Element;

	class Materials
	{
		static std::vector<Materials*> materialVector;
	
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
				pMaterialBuffer = Helper::InitConstantBuffer<BJEStruct::ConstantMaterialBuffer>(GP::GetDevice());

			GP::GetDeviceContext()->PSSetConstantBuffers(2, 1, &pMaterialBuffer);
		}

		int GenRenderPriority();

		std::string GetName() const;

		// FILE SYSTEM

		static std::vector<Materials*> GetAllMaterials()
		{
			return materialVector;
		}

		BJEStruct::MaterialDesc GetMaterialDesc() const
		{
			return cmbDesc.matDesc;			
		};

		const std::string GetTexturePath(MATERIAL_TYPE textureType);

		////////////////////////// 
	private:

		std::string name;

		void Init();

		std::unordered_map<MATERIAL_TYPE, Textures*> textures;
		
		bool isReflect;

		BJEStruct::ConstantMaterialBuffer cmbDesc;

		bool isInit;

		void HasAlphaChannel(ID3D11ShaderResourceView* textureSRV);
	};


}