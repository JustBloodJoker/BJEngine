#include "Drawable.h"

namespace BJEngine
{


	void Drawable::UpdateMatrix()
	{
		if(isSet)
		{
			world = dx::XMMatrixScaling(scale.x, scale.y, scale.z) *
				dx::XMMatrixRotationX(rot.x) * dx::XMMatrixRotationY(rot.y) * dx::XMMatrixRotationZ(rot.z) *
				dx::XMMatrixTranslation(pos.x, pos.y, pos.z);
			isSet = false;
		}

		if (script)
		{
			world *= script->DrawScript();
		}

	}

	void Drawable::CloseS()
	{
		CLOSE(script);
	}

	void Drawable::SetRotateX(float par)
	{
		rot.x = par;
		isSet = true;
	}

	void Drawable::SetRotateY(float par)
	{
		rot.y = par;
		isSet = true;
	}

	void Drawable::SetRotateZ(float par)
	{
		rot.z = par;
		isSet = true;
	}

	void Drawable::SetScaleX(float par)
	{
		scale.x = par;
		isSet = true;
	}

	void Drawable::SetScaleY(float par)
	{
		scale.y = par;
		isSet = true;
	}

	void Drawable::SetScaleZ(float par)
	{
		scale.z = par;
		isSet = true;
	}

	void Drawable::SetChangePosX(float par)
	{
		pos.x = par;
		isSet = true;
	}

	void Drawable::SetChangePosY(float par)
	{
		pos.y = par;
		isSet = true;
	}

	void Drawable::SetChangePosZ(float par)
	{
		pos.z = par;
		isSet = true;
	}

	dx::XMFLOAT3 Drawable::GetPos() const
	{
		return pos;
	}

	dx::XMFLOAT3 Drawable::GetRotate() const
	{
		return rot;
	}

	dx::XMFLOAT3 Drawable::GetScale() const
	{
		return scale;
	}

	void Drawable::CreateScript(Script* script)
	{
		this->script = script;
	}

}