#include "pch.h"
#include "Animation.h"

namespace BJEngine
{


	Animation::Animation() : node(), animation()
	{

	}

	void Animation::SetAnimationParam(aiAnimation* animation, aiNode* node)
	{
		this->animation = animation;
		this->node = node;
	}

	void Animation::SetObjectMatrix(dx::XMMATRIX* objectMatrix)
	{
		this->pObjectMatrix = objectMatrix;
	}

	void Animation::DrawAnimation()
	{
		dx::XMMATRIX Identity = dx::XMMatrixIdentity();

		float TicksPerSecond = (float)(animation->mTicksPerSecond != 0 ? animation->mTicksPerSecond : 25.0f);
		float TimeInTicks = Time() * TicksPerSecond;
		float AnimationTime = fmod(TimeInTicks, animation->mDuration);


	}

	double Animation::Time()
	{
		LARGE_INTEGER currentTime, frequency;
		QueryPerformanceCounter(&currentTime);
		QueryPerformanceFrequency(&frequency);
		double elapsedTime = 0.0f;
		//	double elapsedTime = static_cast<double>(currentTime.QuadPart - BJEUtils::StartTime.QuadPart) / frequency.QuadPart;
		return elapsedTime;
	}


}