#pragma once
#include "pch.h"
#include "Object.h"


namespace BJEngine {
 
    
    class Model :
        public Object
    {
    public:
        Model();
        Model(const char* fbxFileName);
        ~Model();

        bool Init() override;
        void Draw() override;
        void Close() override;
    private:

        struct AdditionalConstantBuffer
        {
            dx::XMFLOAT4 diffuse;
            BOOL hasText;
            BOOL hasNormalMap;
            BOOL pad;
            BOOL pad1;
        };
        ID3D11Buffer* pAdditionalBuffer;
        std::vector<AdditionalConstantBuffer> addConstBuffer;

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
            dx::XMFLOAT3 tangent;
        };

        struct Material
        {
            std::wstring textKDName;
            std::wstring textBumpName;
            dx::XMFLOAT4 difColor;
            bool transparent;
            Textures* texture;
            Textures* textureBump;
        };

        bool LoadModel();
        
        std::vector<std::vector<Vertex>> packVertices;
        std::vector<std::vector<WORD>> packindices;
        std::vector<int> packMaterials;

        std::vector<ID3D11Buffer*> packpVertexBuffer;
        std::vector<ID3D11Buffer*> packpIndexBuffer;

        std::vector<Material> materials;
        const char* filename;
    };


}