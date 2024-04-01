#pragma once
#include "pch.h"
#include "Script.h"
#include "Camera.h"
#include "Textures.h"
#include "Blend.h"
#include "Materials.h"
#include "Element.h"

namespace BJEngine {

	class Materials;
	class BaseElement;

	class Object
	{
	public:

		~Object();

		virtual void Close();

		virtual bool Init();
		
		void SetTexturesPrefixPath(std::wstring prefPath);

		bool IsInited() { return isInited; }

		std::vector<BaseElement*>& MoveElements() { return elements; };

	protected:
	
		bool isInited = false;

		std::vector<Materials*> materials;
		std::vector<BaseElement*> elements;

		std::wstring texturePrefixPath = L"";
	};

}
