#pragma once
#include "pch.h"


namespace BJEStruct
{

	struct ModelVertex
	{
        ModelVertex() {}
        ModelVertex(float x, float y, float z,
            float u, float v,
            float nx, float ny, float nz)
            : pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

        dx::XMFLOAT3 pos;
        dx::XMFLOAT2 texCoord;
        dx::XMFLOAT3 normal;
        dx::XMFLOAT3 tangent;
        dx::XMFLOAT3 bitangent;
	};

    struct VertexSimple
    {
        VertexSimple() {}
        VertexSimple(float x, float y, float z,
            float u, float v,
            float nx, float ny, float nz)
            : pos(x, y, z), texCoord(u, v), normal(nx, ny, nz) {}

        dx::XMFLOAT3 pos;
        dx::XMFLOAT2 texCoord;
        dx::XMFLOAT3 normal;
    };

    struct VertexBackGround
    {
        VertexBackGround() {}
        VertexBackGround(float x, float y, float z,
            float u, float v)
            : pos(x, y, z), texCoord(u, v) {}

        dx::XMFLOAT3 pos;
        dx::XMFLOAT2 texCoord;
    };

}
