#pragma once
#include "pch.h"

namespace BJEngine
{


	class Animation
	{
	public:
		
		Animation();

		void SetAnimationParam(aiAnimation* animation, aiNode* node);
		void SetObjectMatrix(dx::XMMATRIX* objectMatrix);
		void DrawAnimation();

	private:
		double Time();
		


		bool isinit = false;
		
		aiAnimation* animation;
		aiNode* node;
		dx::XMMATRIX* pObjectMatrix;
	};


}
