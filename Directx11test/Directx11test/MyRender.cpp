#include "MyRender.h"
void MyRender::InitParams()
{
	shader = new BJEngine::Shader(L"shaders\\ObjectShaderPS.hlsl", L"shaders\\ObjectShaderVS.hlsl", "VS", "PS");
	shader2 = new BJEngine::Shader(L"shaders\\ObjectShaderPS.hlsl", L"shaders\\ObjectShaderVS.hlsl", "VS", "PS");
	shader3 = new BJEngine::Shader(L"shaders\\ObjectShaderPS.hlsl", L"shaders\\ObjectShaderVS.hlsl", "VS", "PS");
	shader4 = new BJEngine::Shader(L"shaders\\CubeMapShader.txt", L"shaders\\CubeMapShader.txt", "SKYMAP_VS", "SKYMAP_PS");

	texture2 = new BJEngine::Textures(L"models\\textures\\grass.jpg");
	texture3 = new BJEngine::Textures(L"models\\textures\\Texture6.dds");

	pld = new BJEngine::LightDesc();
	pld->pos = dx::XMFLOAT4(-10.0f, 10.0f, 0.0f,1.0f);
	pld->att = dx::XMFLOAT3(0.0f, 0.2f, 0.0f);
	pld->color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pld->enabled = true;

	BJEngine::LightDesc* pld2 = new BJEngine::LightDesc();
	pld2->pos = dx::XMFLOAT4(0.0f, 10.0f, 0.0f, 1.0f);
	pld2->att = dx::XMFLOAT3(0.0f, 0.2f, 0.0f);
	pld2->color = dx::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f);
	pld2->enabled = true;

	sld = new BJEngine::LightDesc();
	sld->pos = dx::XMFLOAT4(0.0f, 5.0f, -10.0f, 1.0f);
	sld->dir = dx::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);
	sld->angle = 20.0f;
	sld->att = dx::XMFLOAT3(0.4f, 0.0f, 0.0f);
	sld->color = dx::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f);
	sld->enabled = true;

//	SetLight(sld, BJEUtils::SPOTLIGHT);
//	SetLight(pld2, BJEUtils::POINTLIGHT);
	SetLight(pld, BJEUtils::POINTLIGHT);
	
	map = new BJEngine::Map();
//	map->SetTerrainTexture("models\\heightmap.bmp");
	map->SetShader(shader);
	map->SetTexture(texture2);

	bg = new BJEngine::BackGround();
	bg->SetTexture(texture3);
	bg->SetShader(shader4);
		
	obj = new BJEngine::Model("models\\wooden.obj");
	obj->SetShader(shader2);

	fbx = new BJEngine::Model("scene.gltf");
	fbx->SetShader(shader3);

	InitObjs(dynamic_cast<BJEngine::Object*>(map));
	InitObjs(dynamic_cast<BJEngine::Object*>(obj));
	InitObjs(dynamic_cast<BJEngine::Object*>(fbx));
	InitObjs(dynamic_cast<BJEngine::Object*>(bg));

	map->SetObjectMatrixScale(500.0f, 10.0f, 500.0f);
	map->SetObjectMatrixPos(0.0f, 10.0f, 0.0f);

	//map->SetObjectMatrixPos(-20.0f, -20.0f, 0.0f);

	obj->SetObjectMatrixPos(7.0f, 0.0f, 7.0f);

	fbx->SetObjectMatrixRotationX(M_PI * 3 / 2);
	fbx->SetObjectMatrixScale(0.01f, 0.01f, 0.01f);
	fbx->SetObjectMatrixPos(0.0f, 0.0f, 7.0f);
}

bool MyRender::DrawActions()
{

	obj->Draw();
	
	map->Draw();

	fbx->Draw();
	


	bg->Draw();
	
	return true;
}
