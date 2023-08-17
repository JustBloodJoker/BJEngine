#include <iostream>
#include "FWork.h"

int main() {
		
	BJFrameWork::FWork frame;
	BJEngine::Render* render = new BJEngine::Render();
	
	frame.SetRender(render);

	frame.Init(L"Hello", L"World", WIDTH, HEIGHT, BJEUtils::_WINDOWED);

	BJEngine::Shader* shader = new BJEngine::Shader(L"shader.txt", L"shader.txt", "VS", "PS");
	BJEngine::Textures* texture2 = new BJEngine::Textures(L"grass.jpg");

	BJEngine::DirectionalLightDesc *ld = new BJEngine::DirectionalLightDesc();
	ld->dir = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	ld->ambient = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	ld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	BJEngine::PointLightDesc* pld = new BJEngine::PointLightDesc();
	pld->pos = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	pld->range = 100.0f;
	pld->att = dx::XMFLOAT3(0.0f, 0.2f, 0.0f);
	pld->ambient = dx::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	pld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	BJEngine::SpotLightDesc* sld= new BJEngine::SpotLightDesc();
	sld->pos = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	sld->dir = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);
	sld->range = 1000.0f;
	sld->cone = 20.0f;
	sld->att = dx::XMFLOAT3(0.4f, 0.02f, 0.0f);
	sld->ambient = dx::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	sld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	BJEngine::Map* map = new BJEngine::Map();
	//map->SetPointLight(pld);
	map->SetSpotLight(sld);
	//map->SetDirectionLight(ld);
	map->SetShader(shader);
	map->SetTexture(texture2);

	BJEngine::BackGround* bg = new BJEngine::BackGround();
	bg->SetTexture(new BJEngine::Textures(L"Texture6.dds"));
	bg->SetShader(new BJEngine::Shader(L"shader.txt", L"shader.txt", "SKYMAP_VS", "SKYMAP_PS"));

	render->InitObjs(dynamic_cast<BJEngine::Object*>(map));
	render->InitObjs(dynamic_cast<BJEngine::Object*>(bg));
	
	frame.Run();

	frame.Close();

	return 0;
}
