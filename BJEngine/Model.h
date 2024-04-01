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
        void Close() override;
       
    private:

        bool LoadModel();       
        std::string filename;

    };


}