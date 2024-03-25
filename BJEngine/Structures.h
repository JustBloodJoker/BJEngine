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
    
    struct VertexPosOnly
    {
        VertexPosOnly() {}
        VertexPosOnly(float x, float y, float z)
            : pos(x, y, z){}

        dx::XMFLOAT3 pos;
    };

    struct WVPConstantBuffer
    {
        dx::XMMATRIX WVP;
    };

    struct VertexConstantBuffer
    {
        dx::XMMATRIX WVP;
        dx::XMMATRIX World;
    };

    struct GeometryConstantBuffer
    {
        float time;
        int explode;
        int pad;
        int pad2;
    };

    struct MainSceneProcessingBuffer
    {
        float gamma;
        float expourse;
        float itens;
        int pad1;
    };

    struct CameraConstantBuffer
    {
        dx::XMMATRIX eyeMatrix;
        dx::XMFLOAT4 eyePos;
    };

    struct MaterialDesc
    {
        MaterialDesc()
        {
            ZeroMemory(this, sizeof(MaterialDesc));
        }

        dx::XMFLOAT4 emissive;
        dx::XMFLOAT4 ambient;
        dx::XMFLOAT4 diffuse;
        dx::XMFLOAT4 specular;

        float specularPower;

        int isTexture;
        int isNormalTexture;
        int isRoughnessTexture;
        int isEmissionTexture;
        int isSpecularTexture;
        int ishaveTransparency;

        int ishavealphablend;


    };

    struct ConstantMaterialBuffer
    {
        MaterialDesc matDesc;
    };

    struct LightDesc
    {

        LightDesc()
        {
            ZeroMemory(this, sizeof(LightDesc));
            color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
            dir = dx::XMFLOAT4(-1.0f, 0.0f, -1.0f, 1.0f);
            pos = dx::XMFLOAT4(0.0f, 500.0f, 0.0f, 1.0f);
            att = dx::XMFLOAT3(0.01f, 0.02f, 0.0f);
            angle = M_PI / 8;
            enabled = 1;
            shadowEnabled = 0;
            lightType = 1;
        }

        dx::XMFLOAT4 pos;
        dx::XMFLOAT4 dir;
        dx::XMFLOAT4 color;

        dx::XMFLOAT3 att;
        float angle;

        int lightType;
        int enabled;
        int shadowEnabled;
        int pad1;

    };

    struct ConstantBufferLight
    {
        int lightsCount = 0;
        int enablePBR = 0;
        int enableIBR = 0;
        int pad2;
    };

    struct CubeGenerateConstantBuffer
    {
        dx::XMMATRIX shViewProjection[6];
    };
}

