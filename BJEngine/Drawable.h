#pragma once
#include "pch.h"
#include "Script.h"

namespace BJEngine
{


	class Drawable
	{

	public:

		Drawable() : world(dx::XMMatrixIdentity()), script(nullptr), pos(dx::XMFLOAT3(0,0,0)),
																	 rot(dx::XMFLOAT3(0, 0, 0)), 
																	 scale(dx::XMFLOAT3(1, 1, 1)) {};
		void CloseS();
	
		void SetRotateX(float par);
		void SetRotateY(float par);
		void SetRotateZ(float par);

		void SetScaleX(float par);
		void SetScaleY(float par);
		void SetScaleZ(float par);

		void SetChangePosX(float par);
		void SetChangePosY(float par);
		void SetChangePosZ(float par);

		dx::XMFLOAT3 GetPos() const ;
		dx::XMFLOAT3 GetRotate()const;
		dx::XMFLOAT3 GetScale()const;

		void CreateScript(Script* script);

		virtual const std::string& GetName() const = 0;

	protected:

		dx::XMMATRIX world;
		virtual void UpdateMatrix() final;

	private:

		Script* script;

		dx::XMFLOAT3 pos;
		dx::XMFLOAT3 rot;
		dx::XMFLOAT3 scale;

		bool isSet = false;
	};


}