#pragma once
#include "FWork.h"

class MyRender : public BJEngine::Render
{
public:
	MyRender() = default;
	~MyRender() = default;

	void InitParams();
	bool Draw() override;
	bool InitObjs(BJEngine::Object* object);
	void Close() override;

private:

	BJEngine::Shader* shader;
	BJEngine::Shader* shader2;
	BJEngine::Textures* texture2;

	BJEngine::DirectionalLightDesc* ld;
	BJEngine::PointLightDesc* pld;
	BJEngine::SpotLightDesc* sld;
	BJEngine::Map* map;
	BJEngine::BackGround* bg;
	BJEngine::StaticModelOBJ* model;

	std::vector<BJEngine::Object*> obj;

};

