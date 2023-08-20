#include "MyRender.h"

void MyRender::InitParams()
{
	shader = new BJEngine::Shader(L"shaders\\shader.txt", L"shaders\\shader.txt", "VS", "PS");
	shader2 = new BJEngine::Shader(L"shaders\\objMeshShader.txt", L"shaders\\objMeshShader.txt", "VS", "PS");
	texture2 = new BJEngine::Textures(L"models\\textures\\grass.jpg");

	ld = new BJEngine::DirectionalLightDesc();
	ld->dir = dx::XMFLOAT3(0.0f, 1.0f, 0.0f);
	ld->ambient = dx::XMFLOAT4(0.1f, 0.1f, 0.1f, 1.0f);
	ld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	pld = new BJEngine::PointLightDesc();
	pld->pos = dx::XMFLOAT3(0.0f, 0.0f, 0.0f);
	pld->range = 100.0f;
	pld->att = dx::XMFLOAT3(0.0f, 0.2f, 0.0f);
	pld->ambient = dx::XMFLOAT4(0.3f, 0.3f, 0.3f, 1.0f);
	pld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	sld = new BJEngine::SpotLightDesc();
	sld->pos = dx::XMFLOAT3(0.0f, 5.0f, -10.0f);
	sld->dir = dx::XMFLOAT3(0.0f, 0.0f, 1.0f);
	sld->range = 1000.0f;
	sld->cone = 20.0f;
	sld->att = dx::XMFLOAT3(0.9f, 0.02f, 0.0f);
	sld->ambient = dx::XMFLOAT4(0.2f, 0.2f, 0.2f, 1.0f);
	sld->diffuse = dx::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);

	map = new BJEngine::Map();
	map->SetSpotLight(sld);
	map->SetShader(shader);
	map->SetTexture(texture2);

	bg = new BJEngine::BackGround();
	bg->SetTexture(new BJEngine::Textures(L"models\\textures\\Texture6.dds"));
	bg->SetShader(new BJEngine::Shader(L"shaders\\shader.txt", L"shaders\\shader.txt", "SKYMAP_VS", "SKYMAP_PS"));

	model = new BJEngine::StaticModelOBJ(L"models\\wooden watch tower21.obj");
	model->SetShader(shader2);
	model->SetSpotLight(sld);

	InitObjs(dynamic_cast<BJEngine::Object*>(map));
	InitObjs(dynamic_cast<BJEngine::Object*>(model));
	InitObjs(dynamic_cast<BJEngine::Object*>(bg));


}

bool MyRender::Draw()
{
	BeginFrame();
	GetContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	GetCamera()->CameraMove(GetInput());


	for (auto el : obj) {
		el->SetViewAndProjectionMatrix(GetCamera()->GetViewMatrix(), GetProjMatrix());
		
	}
	obj[0]->Draw();
	

	obj[1]->Draw();
	obj[1]->SetObjectMatrixPos(0.0f, -0.68f, 0.0f);
	obj[1]->SetObjectMatrixRotation(3.14f / 90.0f);

	obj[2]->Draw();

	EndFrame();

	return true;
}

bool MyRender::InitObjs(BJEngine::Object* object)
{
	object->SetCamera(GetCamera());
	object->SetDevice(GetDevice());
	object->SetDeviceContext(GetContext());

	if (!object->Init()) {
		return false;
	};

	obj.push_back(object);

	return true;
}

void MyRender::Close()
{
	Render::Close();
	for (auto& el : obj) {
		CLOSE(el);
	}

}


