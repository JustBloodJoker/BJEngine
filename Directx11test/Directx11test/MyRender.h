#pragma once
#include "FWork.h"

class MyRender : public BJEngine::Render
{
public:
	MyRender() = default;
	~MyRender() = default;

	void InitParams();
	bool DrawActions() override;

private:

	BJEngine::Shader* shader;
	
	BJEngine::Model* model;

	BJEngine::Textures* textureCubeMap;

	BJEngine::LightDesc* pld;
	BJEngine::LightDesc* sld;
	BJEngine::LightDesc* ld;
	BJEngine::LightDesc* sld2;
	BJEngine::LightDesc* pld2;
	BJEngine::BackGround* bg;
	BJEngine::Model* fbx;
	BJEngine::Model* fbx1;
};

