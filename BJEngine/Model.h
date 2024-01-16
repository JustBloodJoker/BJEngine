#pragma once
#include "Object.h"
#include "Animation.h"
#include "Element.h"

namespace BJEngine {


    class Model :
        public Object
    {

        friend class UI;

    public:
        Model();
        Model(const char* FileName);
        ~Model();

        bool Init() override;
        void Draw() override;
        void Close() override;
        void MinDraw() override;

        void SetScript(std::string data);
    private:

        std::vector<Element*> elements;

        dx::XMMATRIX bworld;
        dx::XMMATRIX outLine;

        bool LoadModel();

        std::vector<BJEStruct::ModelVertex> ver;
        std::vector<WORD> ind;
        Materials* material;
        dx::XMVECTOR min;
        dx::XMVECTOR max;

        std::string filename;

        aiNode* nodd;
        bool isAnimation = false;
        bool InitAnimation();

        

        const aiScene* scene = nullptr;
        Assimp::Importer importer;

        char buffer[100];
        bool isSimulate = false, returned = false;
    };


}