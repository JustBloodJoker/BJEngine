#pragma once
#include "Object.h"


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
        void Draw(const CameraDesc cam) override;
        void Close() override;
        void MinDraw(dx::BoundingFrustum frustum) override;

        void SetScript(std::string data);
    private:


        std::vector<Element*> elements;

        dx::XMMATRIX bworld;
  
        bool LoadModel();

        std::vector<BJEStruct::ModelVertex> ver;
        std::vector<WORD> ind;
        dx::XMVECTOR min;
        dx::XMVECTOR max;

        std::string filename;

        //char buffer[100];
        bool isSimulate = false, returned = false;


    };


}