#pragma once
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

    private:


        Vertex* vertices;
        WORD* indices;


    };

}