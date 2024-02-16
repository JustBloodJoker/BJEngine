#include "Object.h"

namespace BJEngine {

	Object::Object()
	{
		
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

			RELEASE(ConstantBuffers);

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

	

	void Object::SetTexturesPrefixPath(std::wstring prefPath)
	{
		this->texturePrefixPath += prefPath;
	}



	


}