#pragma once
#include "pch.h"
#include "Shader.h"
#include "Textures.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Camera.h"

namespace BJEngine {


	class Object
	{
	private:

		friend class Cube;
		friend class Map;
		friend class BackGround;

	public:

		Object();
		Object(float x, float y, float z);
		~Object();

		virtual void Close();

		virtual void Draw();
		virtual bool Init();

		bool InitIsLightConstantBuffer();
		void DrawIsLightConstantBuffer();

		struct Vertex
		{
			Vertex() {}
			Vertex(float x, float y, float z,
				float u, float v,
				float nx, float ny, float nz)
				: pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

			dx::XMFLOAT3 pos;
			dx::XMFLOAT2 texCoord;
			dx::XMFLOAT3 normal;
		};

		struct ConstantBuffer
		{
			dx::XMMATRIX WVP;
			dx::XMMATRIX  World;
		};

		struct IsLightsConstantBuffer
		{
			BOOL isDirLight;
			BOOL isPointLight;
			BOOL isSpotLight;
			int pad;
		};

		void SetCamera(Camera* cam);
		void SetRastVal(bool choose);
		void SetShader(Shader* shader);
		void SetTexture(Textures* texture);
		void SetTransparency(bool choose, float* blendFactor);
		void SetDirectionLight(DirectionalLightDesc* lightdesc);
		void SetPointLight(PointLightDesc* lightdesc);
		void SetSpotLight(SpotLightDesc* lightdesc);
		void SetDevice(ID3D11Device* pd3dDevice);
		void SetDeviceContext(ID3D11DeviceContext* pImmediateContext);
		void SetViewAndProjectionMatrix(dx::XMMATRIX view, dx::XMMATRIX projection);

		virtual bool HasTexture() final { return hastext; };

	private:
		Camera* cam;
		Light* light = nullptr;
		Textures* texture;
		Shader* shader;


		IsLightsConstantBuffer istypeoflight;
		ID3D11Buffer* ilcb;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11Buffer* pConstantBuffer;

		dx::XMMATRIX world;
		dx::XMMATRIX view;
		dx::XMMATRIX projection;

		bool hastext = false;

		ID3D11Device* pd3dDevice;
		ID3D11DeviceContext* pImmediateContext;

		bool isRasterized = false;
		ID3D11RasterizerState* wireFrame;
		D3D11_RASTERIZER_DESC wfdesc;
		HRESULT IsRasterizedObj();
		void DrawRasterized();

		ID3D11BlendState* transparency;
		float* blendFactor;
		bool isTransparency = false;
		HRESULT IsTransparencyObj();
		void DrawTransparency();


		D3D11_RASTERIZER_DESC cmdesc;
	};


}
