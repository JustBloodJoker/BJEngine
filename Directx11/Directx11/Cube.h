#pragma once
#include "pch.h"
#include "Object.h"


namespace BJEngine {

    struct CubeDesc;
   


    class Cube :
        public Object
    {
    public:

        Cube(CubeDesc desc);
        Cube();
        ~Cube();

        void Draw() override;
        bool Init() override;
        void Close() override;

        struct Vertex 
        {
            Vertex() {}
            Vertex(float x, float y, float z,
                float cr, float cg, float cb, float ca)
                : pos(x, y, z), color(cr, cg, cb, ca) {}

            dx::XMFLOAT3 pos;
            dx::XMFLOAT4 color;
        };
    private:

        Vertex* vertices;
        WORD* indices;

    };

    struct CubeDesc {

        Cube::Vertex vertices[8] = {
                     Cube::Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f, 1.0f),
                     Cube::Vertex(-1.0f, +1.0f, -1.0f, 0.0f, 1.0f, 0.0f, 1.0f),
                     Cube::Vertex(+1.0f, +1.0f, -1.0f, 0.0f, 0.0f, 1.0f, 1.0f),
                     Cube::Vertex(+1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f),
                     Cube::Vertex(-1.0f, -1.0f, +1.0f, 0.0f, 1.0f, 1.0f, 1.0f),
                     Cube::Vertex(-1.0f, +1.0f, +1.0f, 1.0f, 1.0f, 1.0f, 1.0f),
                     Cube::Vertex(+1.0f, +1.0f, +1.0f, 1.0f, 0.0f, 1.0f, 1.0f),
                     Cube::Vertex(+1.0f, -1.0f, +1.0f, 1.0f, 0.0f, 0.0f, 1.0f), };

        WORD indices[36] = {

                0, 1, 2,
                0, 2, 3,

                4, 6, 5,
                4, 7, 6,

                4, 5, 1,
                4, 1, 0,

                3, 2, 6,
                3, 6, 7,

                1, 5, 6,
                1, 6, 2,

                4, 0, 3,
                4, 3, 7
        };
    };


}
