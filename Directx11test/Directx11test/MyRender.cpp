#include "MyRender.h"
void MyRender::InitParams()
{
	shader = new BJEngine::Shader(L"shaders\\ObjectShader.txt", L"shaders\\ObjectShader.txt", "VS", "PS");
	shader2 = new BJEngine::Shader(L"shaders\\ObjectShader.txt", L"shaders\\ObjectShader.txt", "VS", "PS");
	shader3 = new BJEngine::Shader(L"shaders\\ObjectShader.txt", L"shaders\\ObjectShader.txt", "VS", "PS");
	shader4 = new BJEngine::Shader(L"shaders\\CubeMapShader.txt", L"shaders\\CubeMapShader.txt", "SKYMAP_VS", "SKYMAP_PS");

	texture2 = new BJEngine::Textures(L"models\\textures\\grass.jpg");
	texture3 = new BJEngine::Textures(L"models\\textures\\Texture6.dds");

	ld = new BJEngine::LightDesc();
	ld->dir = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	ld->ambient = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	ld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	pld = new BJEngine::LightDesc();
	pld->pos = dx::XMFLOAT3(10.0f, 0.0f, 0.0f);
	pld->range = 100.0f;
	pld->att = dx::XMFLOAT3(0.0f, 0.2f, 0.0f);
	pld->ambient = dx::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	pld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	sld = new BJEngine::LightDesc();
	sld->pos = dx::XMFLOAT3(0.0f, 5.0f, -10.0f);
	sld->dir = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);
	sld->range = 1000.0f;
	sld->cone = 20.0f;
	sld->att = dx::XMFLOAT3(0.9f, 0.02f, 0.0f);
	sld->ambient = dx::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	sld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	//SetLight(ld, BJEUtils::DIRECTIONALLIGHT);
	SetLight(sld, BJEUtils::SPOTLIGHT);
	SetLight(pld, BJEUtils::POINTLIGHT);
	
	map = new BJEngine::Map();
	//map->SetTerrainTexture("models\\heightmap.bmp");
	map->SetShader(shader);
	map->SetTexture(texture2);

	bg = new BJEngine::BackGround();
	bg->SetTexture(texture3);
	bg->SetShader(shader4);

	obj = new BJEngine::Model("models\\wooden.obj");
	obj->SetShader(shader2);

	fbx = new BJEngine::Model("models\\Wolf.fbx");
	fbx->SetShader(shader3);

	InitObjs(dynamic_cast<BJEngine::Object*>(map));
	InitObjs(dynamic_cast<BJEngine::Object*>(obj));
	InitObjs(dynamic_cast<BJEngine::Object*>(fbx));
	InitObjs(dynamic_cast<BJEngine::Object*>(bg));

	
}

bool MyRender::DrawActions()
{
	for (float i = -100.0f; i < 100.0f; i += 10.0f)
	{
		for (float j = -100.0f; j < 100.0f; j += 10.0f)
		{
			obj->SetObjectMatrixPos(i, 0.0f, j);
			obj->Draw();
		}
	}
	
	fbx->SetObjectMatrixRotationX(3.14 * 1.5);
	fbx->SetObjectMatrixScale(4.0f, 4.0f, 4.0f);
	fbx->Draw();

	map->SetObjectMatrixScale(500.0f, 10.0f, 500.0f);
	map->SetObjectMatrixPos(0.0f, 10.0f, 0.0f);
	map->Draw();
	//map->SetObjectMatrixPos(-10.0f, -10.0f, -10.f);


	bg->Draw();
	
	return true;
}
