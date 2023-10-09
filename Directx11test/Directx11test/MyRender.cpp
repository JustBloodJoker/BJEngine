#include "MyRender.h"
void MyRender::InitParams()
{
	//shader = new BJEngine::Shader(L"PSTest.hlsl", L"shaders\\ObjectShaderVS.hlsl", "VS", "PS");
	
	textureCubeMap = new BJEngine::Textures(L"models\\textures\\Texture6.dds");

	pld = new BJEngine::LightDesc();
	pld->pos = dx::XMFLOAT4(-10.0f, 1000.0f, 0.0f,1.0f);
	pld->att = dx::XMFLOAT3(0.0f, 0.005f, 0.0f);
	pld->color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pld->enabled = true;

	pld2 = new BJEngine::LightDesc();
	pld2->pos = dx::XMFLOAT4(0.0f, 250.0f, 0.0f, 1.0f);
	pld2->att = dx::XMFLOAT3(0.0f, 0.005f, 0.0f);
	pld2->color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	pld2->enabled = true;

	sld = new BJEngine::LightDesc();
	sld->pos = dx::XMFLOAT4(50.0f, 500.0f, 0.0f, 1.0f);
	sld->dir = dx::XMFLOAT4(1.0f, 0.0f, 1.0f, 1.0f);
	sld->angle = M_PI/8;
	sld->att = dx::XMFLOAT3(0.0f, 0.0005f, 0.0f);
	sld->color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sld->enabled = true;

//	SetLight(pld2, BJEUtils::POINTLIGHT);
//	SetLight(pld, BJEUtils::POINTLIGHT);

	
	sld2 = new BJEngine::LightDesc();
	sld2->pos = dx::XMFLOAT4(50.0f, 500.0f, 0.0f, 1.0f);
	sld2->dir = dx::XMFLOAT4(-1.0f, 0.0f, -1.0f, 1.0f);
	sld2->angle = M_PI / 8;
	sld2->att = dx::XMFLOAT3(0.0f, 0.0005f, 0.0f);
	sld2->color = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
	sld2->enabled = true;
	
	SetLight(sld, BJEUtils::SPOTLIGHT);

	bg = new BJEngine::BackGround();
	bg->SetTexture(textureCubeMap);

	fbx = new BJEngine::Model("sponza\\Sponza.gltf");
	fbx->SetTexturesPrefixPath(L"sponza\\");

	fbx1 = new BJEngine::Model("scene.gltf");

	InitObjs(dynamic_cast<BJEngine::Object*>(fbx));
	InitObjs(dynamic_cast<BJEngine::Object*>(fbx1));
	InitObjs(dynamic_cast<BJEngine::Object*>(bg));

}

bool MyRender::DrawActions()
{

	fbx->Draw();
	fbx1->Draw();
	
	bg->Draw();

	return true;
}
