#pragma once
#include "FWork.h"

class MyRender : public BJEngine::Render
{
public:
	MyRender() = default;
	~MyRender() = default;

	void InitParams();
	bool Draw() override;
	void Close() override;

private:

	BJEngine::Shader* shader;
	BJEngine::Shader* shader2;
	BJEngine::Shader* shader3;
	BJEngine::Shader* shader4;
	BJEngine::Textures* texture2;
	BJEngine::Textures* texture3;

	BJEngine::LightDesc* ld;
	BJEngine::LightDesc* pld;
	BJEngine::LightDesc* sld;
	BJEngine::Map* map;
	BJEngine::BackGround* bg;
	BJEngine::Model* obj;
	BJEngine::Model* fbx;


	std::vector<BJEngine::Object*> objects;

};

