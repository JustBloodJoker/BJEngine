#pragma once
#include "pch.h"
#include "Object.h"
#include "Animation.h"

namespace BJEngine {
 
    
    class Model :
        public Object
    {
    public:
        Model();
        Model(const char* FileName);
        ~Model();

        bool Init() override;
        void Draw() override;
        void Close() override;
        void MinDraw(dx::XMMATRIX matrix, dx::XMMATRIX mat2) override;
    private:


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

        bool LoadModel();
        
        std::vector<std::vector<Vertex>> packVertices;
        std::vector<std::vector<WORD>> packindices;
        std::vector<int> packMaterials;

        std::vector<ID3D11Buffer*> packpVertexBuffer;
        std::vector<ID3D11Buffer*> packpIndexBuffer;

        const char* filename;

        std::vector<Animation> animation;
        aiNode* nodd;
        bool isAnimation = false;
        bool InitAnimation();

        std::vector<dx::XMVECTOR> minExtentLocal;
        std::vector<dx::XMVECTOR> maxExtentLocal;

        const aiScene* scene = nullptr;
        Assimp::Importer importer;
    };


}