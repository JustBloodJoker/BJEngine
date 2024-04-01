#include "Object.h"

namespace BJEngine {


	Object::~Object()
	{

	}

	void Object::Close()
	{
		if (isInited)
		{
			isInited = false;

			for (auto& material : materials)
			{
				CLOSE(material);
			}
			materials.clear();
		}
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