#pragma once 
#include "pch.h"
#include "Textures.h"

namespace BJEngine
{
	enum
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
		HAS_SPECULAR_TEXTURE
	};



	class Materials
	{


	public:

		Materials(ID3D11Device* pd3dDevice);

		void SetParam(short paramType, dx::XMFLOAT4 param);
		void SetParam(short paramType, float param);

		void SetTexture(short textureType, std::wstring textureName, ID3D11Device* pd3dDevice);
		void SetTexture(short textureType, Textures* texture, ID3D11Device* pd3dDevice);

		void Draw(ID3D11DeviceContext* pImmediateContext, int registerMaterialPos);

		void Close();
	private:

		void Init(ID3D11Device* pd3dDevice);

		Textures* texture;
		Textures* normalTexture;
		Textures* roughnessTexture;
		Textures* emissionTexture;
		Textures* specularTexture;

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