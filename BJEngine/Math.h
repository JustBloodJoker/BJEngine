#pragma once
#include <cmath>
#include <random>

namespace BJEngine
{


	class Math
	{
		Math() = default;

		static bool isInited;
		
	public:

		template <typename TYPE>
		static TYPE RandomizeData(TYPE min, TYPE max);
		

	private:

		static void InitMath();
		
		static std::random_device rd;
		static std::mt19937 gen;

	};


	template<typename TYPE>
	inline TYPE Math::RandomizeData(TYPE min, TYPE max)
	{
		if(!isInited)
			InitMath();

		std::uniform_real_distribution<TYPE> dis(min, max);

		return dis(gen);
	}

}