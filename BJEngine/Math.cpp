#include "Math.h"

namespace BJEngine
{
	bool Math::isInited = false;
	std::random_device Math::rd;
	std::mt19937 Math::gen;




	void Math::InitMath()
	{
		gen = std::mt19937(rd());
		isInited = true;
	}




}