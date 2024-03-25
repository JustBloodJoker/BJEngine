#pragma once
#include "pch.h"
#include "Materials.h"

namespace BJEngine
{

    class Materials;

    enum ELEMENT_PRIORITY
    {
        DEFAULT = 1,
        TRANSPARENCY = 2,
        SKYBOX = 3,
    };

    class BaseElement
    {
        
        static ID3D11Buffer* pConstantBuffer;

    public:

        static void BindConstantBuffer();
        static ID3D11Buffer*& GetConstantBuffer();
        
       
        virtual void Close()                                   = 0;
        virtual void Init()                                    = 0;
        virtual void Draw(CameraDesc cam,
            ID3D11DeviceContext* context)                      = 0;
        virtual void DrawShadow(ID3D11DeviceContext* context) = 0;
       
        const bool operator<(const BaseElement& other) const 
        {
            return priority > other.GetPriorityRender();
        }

        const ELEMENT_PRIORITY& GetPriorityRender() const
        {
            return priority;
        };

        const std::string& GetName() const
        {
            return name;
        }


    protected:

        ELEMENT_PRIORITY priority = DEFAULT;
        std::string name;

    };

	class Element
        : public BaseElement
	{
       

        static int count;
        
	public:
        
        
        Element();
        Element(std::vector<BJEStruct::ModelVertex>&& v, std::vector<WORD>&& i, Materials* material,
                    dx::XMVECTOR min, dx::XMVECTOR max);
        ~Element();

        void Close()                        override;
        void Init()                         override;
        void Draw(CameraDesc cam,
            ID3D11DeviceContext* context)             override;
        void DrawShadow(ID3D11DeviceContext* context)                     override;
        
        std::string GetMaterialName()                          ;
        std::vector<BJEStruct::ModelVertex> GetVertices() const;
        std::vector<WORD>                   GetIndices()  const;

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

    class ElementSkyBox
        : public BaseElement
    {
        CameraDesc lastCamDesc;
        
    public:

        ElementSkyBox(Textures* texture);
        ~ElementSkyBox();

        void Close()                        override;
        void Init()                         override;
        void Draw(CameraDesc cam, ID3D11DeviceContext* context)           override;
        void DrawShadow(ID3D11DeviceContext* context)                   override;

        std::string GetTextureName();
        std::vector<BJEStruct::VertexPosOnly> GetVertices() const;
        std::vector<WORD>                    GetIndices()  const;
        Textures* GetTexture() { return ttext; };

    private:
        dx::XMMATRIX world;

        std::vector<BJEStruct::VertexPosOnly> vertices;
        std::vector<WORD> indices;

        Textures* ttext;

        ID3D11Buffer* pVertexBuffer;
        ID3D11Buffer* pIndexBuffer;

        bool drawing;

       
    };

    class ElementSkyBoxConvertion
        : public BaseElement
    {
        static ID3D11Buffer* cubeBuffer;

    public:

        ElementSkyBoxConvertion();
        ~ElementSkyBoxConvertion();

        void Close()                        override;
        void Init()                         override;
        void Draw(CameraDesc cam, ID3D11DeviceContext* context)           override;
        void DrawShadow(ID3D11DeviceContext* context)                   override;

        void Redraw();
        bool IsDraw();

    private:
        bool isDraw = false;

        
        D3D11_VIEWPORT vp;
        BJEStruct::CubeGenerateConstantBuffer matrices;
    };
}