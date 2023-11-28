#pragma once
#include "pch.h"
#include "Object.h"

namespace BJEngine {


    struct MapDesc {
        Object::Vertex vertices[4] =
        {
            Object::Vertex(-1.0f, -1.0f, -1.0f, 100.0f, 100.0f, 0.0f, 1.0f, 0.0f),
            Object::Vertex(1.0f, -1.0f, -1.0f,   0.0f, 100.0f, 0.0f, 1.0f, 0.0f),
            Object::Vertex(1.0f, -1.0f,  1.0f,   0.0f,   0.0f, 0.0f, 1.0f, 0.0f),
            Object::Vertex(-1.0f, -1.0f,  1.0f, 100.0f,   0.0f, 0.0f, 1.0f, 0.0f),
        };

        WORD indices[6] = {
            0,  1,  2,
            0,  2,  3,
        };
    };


    class Map :
        public Object
    {
    public:
        Map();
        ~Map();

        bool Init() override;
        void Draw() override;
        void Close() override;

        void SetTerrainTexture(const char* filename);
        void InitTerrainTexture();

    private:
        bool isTerrain = false;
        const char* terrainFileName;

        std::vector<Vertex> vertices;
        std::vector<WORD> indices;

        int NumVertices;
        int NumFaces;


        struct HeightMapInfo {
            int terrainWidth;
            int terrainHeight;
            dx::XMFLOAT3* heightMap;
        };
        HeightMapInfo hmInfo;

    };

}