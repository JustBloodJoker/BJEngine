#pragma once
#include "pch.h"
#include "Materials.h"

namespace BJEngine
{

    class Materials;

	class Element
	{
        static ID3D11Buffer* pConstantBuffer;
        static ID3D11Buffer* pWorldConstantBuffer;

        static int count;
        
	public:
        
        static void BindConstantBuffer();

        Element();
        Element(std::vector<BJEStruct::ModelVertex> v, std::vector<WORD> i, Materials* material,
                    dx::XMVECTOR min, dx::XMVECTOR max);
        ~Element();

        void Close();
        void Init();
        void Draw(CameraDesc cam);
        void DrawShadow();
       
        std::string GetName() const;

        void SetFocusState(bool state);

        bool operator<(const Element& other) const {
            return priority > other.priority;
        }

        const int GetPriorityRender() const;

    protected:

        virtual void Binds();
        dx::XMMATRIX world;
        dx::XMVECTOR minLocal;
        dx::XMVECTOR maxLocal;
        dx::BoundingBox objectBox;
        bool frustumCheck = true;

	private:
        int priority = 1;

        bool drawing;

        bool focusedState = false;

        std::string name;

        std::vector<BJEStruct::ModelVertex> vertices;
        std::vector<WORD> indices;
        Materials* pMaterial;

        ID3D11Buffer* pIndexBuffer;
        ID3D11Buffer* pVertexBuffer;

    };


}