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

		Object();
		Object(float x, float y, float z);
		~Object();

		virtual void Close();

		virtual bool Init();
		
		void SetTexturesPrefixPath(std::wstring prefPath);

		bool IsInited() { return isInited; }

		std::vector<BaseElement*>& MoveElements() { return elements; };

	protected:
	
		bool isInited = false;

		
		Script* script;

		ID3D11Buffer* pVertexBuffer;
		ID3D11Buffer* pIndexBuffer;
		ID3D11Buffer* ConstantBuffers;


		std::vector<Materials*> materials;
		std::vector<BaseElement*> elements;

		std::wstring texturePrefixPath = L"";
	};

}
