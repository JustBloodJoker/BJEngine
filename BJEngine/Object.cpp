#include "Object.h"

namespace BJEngine {

	Object::Object()
	{
		world = dx::XMMatrixIdentity();

		pVertexBuffer = nullptr;
		pIndexBuffer = nullptr;
	}

	Object::Object(float x, float y, float z)
	{
		pVertexBuffer = nullptr;
		pIndexBuffer = nullptr;
	}

	Object::~Object()
	{

	}

	void Object::Close()
	{
		if (isInited)
		{
			CLOSE(script);
			RELEASE(pVertexBuffer);
			RELEASE(pIndexBuffer);
			
			for (auto& el : ConstantBuffers)
				RELEASE(el);

			isInited = false;

			for (auto& material : materials)
			{
				CLOSE(material);
			}
			materials.clear();
		}
	}

	void Object::Draw(const CameraDesc cam)
	{
	}


	bool Object::Init()
	{
		return true;
	}

	void Object::MinDraw(dx::BoundingFrustum frustum)
	{
	}


	

	void Object::SetLightViewAndProjectionMatrix(dx::XMMATRIX view, dx::XMMATRIX projecion, int index)
	{
		this->lView[index] = view;
		this->lProjection[index] = projecion;
	}

	void Object::SetObjectMatrixPos(float x, float y, float z)
	{
		pos = dx::XMMatrixTranslation(x, y, z);
	}

	void Object::SetObjectMatrixPos(dx::XMFLOAT3 xmf3)
	{
		pos = dx::XMMatrixTranslation(xmf3.x, xmf3.y, xmf3.z);
	}

	void Object::SetObjectMatrixScale(float x, float y, float z)
	{
		scale = dx::XMMatrixScaling(x, y, z);
	}

	void Object::SetObjectMatrixRotationY(float angle)
	{
		rotation = dx::XMMatrixRotationY(angle);
	}

	void Object::SetObjectMatrixRotationX(float angle)
	{
		rotation = dx::XMMatrixRotationX(angle);
	}

	void Object::SetObjectMatrixRotationZ(float angle)
	{
		rotation = dx::XMMatrixRotationZ(angle);
	}

	void Object::SetTexturesPrefixPath(std::wstring prefPath)
	{
		this->texturePrefixPath += prefPath;
	}



	dx::XMMATRIX Object::GetObjectMatrix()
	{
		return world;
	}


}