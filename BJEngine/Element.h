#pragma once
#include "pch.h"
#include "Materials.h"

namespace BJEngine
{

    class Materials;

	class Element
	{
        static int count;
        
	public:
        
        Element();
        Element(std::vector<BJEStruct::ModelVertex> v, std::vector<WORD> i, Materials* material,
                    dx::XMVECTOR min, dx::XMVECTOR max);
        ~Element();

        void Close();
        void Init(std::vector<ID3D11Buffer*>* ConstantBuffers);
        void Draw(CameraDesc cam, dx::XMMATRIX* lView, dx::XMMATRIX* lProjection);
        void MinDraw(dx::BoundingFrustum frustum);

        std::string GetName() const;

        void SetFocusState(bool state);

        dx::XMFLOAT3 GetWorldPosition() const;

	private:
        dx::BoundingBox objectBox;
        dx::XMMATRIX world;

        bool drawing;

        bool focusedState = false;

        std::string name;

        std::vector<BJEStruct::ModelVertex> vertices;
        std::vector<WORD> indices;
        Materials* pMaterial;

        ID3D11Buffer* pIndexBuffer;
        ID3D11Buffer* pVertexBuffer;
        std::vector<ID3D11Buffer*>* ConstantBuffers;

        dx::XMVECTOR minLocal;
        dx::XMVECTOR maxLocal;

	};


}