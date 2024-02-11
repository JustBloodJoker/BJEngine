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

		static std::vector<BJEngine::Object*>* objects;

	public:

		static void SetObjectPtr(std::vector<BJEngine::Object*>* obj)
		{
			Materials::objects = obj;
		};

		Materials();

		void SetParam(MATERIAL_TYPE paramType, dx::XMFLOAT4 param);
		void SetParam(MATERIAL_TYPE paramType, float param);

		void SetTexture(MATERIAL_TYPE textureType, std::wstring textureName);
		void SetTexture(MATERIAL_TYPE textureType, Textures* texture);

		void Draw(int registerMaterialPos);
		void Draw(int registerMaterialPos, Element* elem);

		void Close();

	private:

		UINT sizeViewPorts = 1;
		CD3D11_VIEWPORT curr_vp;
		static CD3D11_VIEWPORT reflect_vp;

		void Init();

		void GenerateReflectionTexture(dx::XMFLOAT3 pos);

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

		ID3D11Buffer* pMaterialBuffer;

		struct ConstantMaterialBuffer
		{
			MaterialDesc matDesc;
		};

		ConstantMaterialBuffer cmbDesc;

		bool isInit;

		void HasAlphaChannel(ID3D11ShaderResourceView* textureSRV);
	};


}