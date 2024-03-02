#pragma once
#include "pch.h"
#include "Materials.h"

namespace BJEngine
{

    class Materials;

    class BaseElement
    {
        
        static ID3D11Buffer* pConstantBuffer;

    public:

        static void BindConstantBuffer();
        static ID3D11Buffer*& GetConstantBuffer();
        
        
        
        virtual void Close()                                   = 0;
        virtual void Init()                                    = 0;
        virtual void Draw(CameraDesc cam)                      = 0;
        virtual void DrawShadow()                              = 0;
       
        const bool operator<(const BaseElement& other) const 
        {
            return priority > other.GetPriorityRender();
        }

        const int& GetPriorityRender() const
        {
            return priority;
        };

        const std::string& GetName() const
        {
            return name;
        }


    protected:

        int priority = 1;
        std::string name;

    };

	class Element
        : public BaseElement
	{
       

        static int count;
        
	public:
        
        
        Element();
        Element(std::vector<BJEStruct::ModelVertex> v, std::vector<WORD> i, Materials* material,
                    dx::XMVECTOR min, dx::XMVECTOR max);
        ~Element();

        void Close()                        override;
        void Init()                         override;
        void Draw(CameraDesc cam)           override;
        void DrawShadow()                   override;
        
	private:
        
        dx::XMMATRIX world;
        dx::XMVECTOR minLocal;
        dx::XMVECTOR maxLocal;
        dx::BoundingBox objectBox;
        bool frustumCheck = true;

        bool drawing;

        bool focusedState = false;

        std::vector<BJEStruct::ModelVertex> vertices;
        std::vector<WORD> indices;
        Materials* pMaterial;

        ID3D11Buffer* pIndexBuffer;
        ID3D11Buffer* pVertexBuffer;
    };

    class ElementSphere
        : public BaseElement
    {

        static int count;

    public:

        ElementSphere();
        ~ElementSphere();

        void Close()                        override;
        void Init()                         override;
        void Draw(CameraDesc cam)           override;
        void DrawShadow()                   override;
        
    private:

        dx::XMMATRIX world;
        
        bool drawing = false;
        bool drawingInShadowPass = false;

        ID3D11Buffer* pIndexBuffer;
        ID3D11Buffer* pVertexBuffer;

    };


}