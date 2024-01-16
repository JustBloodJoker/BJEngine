#pragma once
#include "pch.h"
#include "Materials.h"
#include "Shader.h"

namespace BJEngine
{


	class Element
	{
        static int count;

	public:
        
        Element();
        Element(std::vector<BJEStruct::ModelVertex> v, std::vector<WORD> i, Materials* material, dx::XMVECTOR min, dx::XMVECTOR max, ID3D11Device* pd3dDevice);
        ~Element();

        void Close();
        void Init(ID3D11Device* pd3dDevice, ID3D11Buffer* pConstantBuffer);
        void Draw(ID3D11DeviceContext* pImmediateContext, UINT* stride, UINT* offset,
                  dx::XMMATRIX mainWorldMatrix, dx::XMMATRIX outLine, dx::XMMATRIX viewMatrix, dx::XMMATRIX projectionMatrix, dx::XMMATRIX* lView, dx::XMMATRIX* lProjection);
        void MinDraw(ID3D11DeviceContext* pImmediateContext, UINT* stride, UINT* offset);

        dx::XMVECTOR GetMinLocal();
        dx::XMVECTOR GetMaxLocal();

        std::string GetName() const;

        void SetFocusState(bool state);

	private:

        bool focusedState = false;

        std::string name;

        std::vector<BJEStruct::ModelVertex> vertices;
        std::vector<WORD> indices;
        Materials* pMaterial;

        ID3D11Buffer* pIndexBuffer;
        ID3D11Buffer* pVertexBuffer;
        ID3D11Buffer* pConstantBuffer;
        ID3D11Buffer* pGlowConstantBuffer;

        dx::XMVECTOR minLocal;
        dx::XMVECTOR maxLocal;

        static Shader* glowShader;

        struct CBuf
        {
            dx::XMMATRIX WVP;
        };

	};


}