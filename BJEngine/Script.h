#pragma once 
#include "pch.h"

namespace BJEngine
{

#define ONCE "once"
#define LOOP "loop"
#define ROTATE "rotate"
#define MOVE "move"
#define X "x"
#define Y "y"
#define Z "z"
#define A "a"

#define REG "reg"
#define EQU "equ"
#define HESH "#"
#define OP "op"

#define ENDS "ends"


	class Script
	{
	public:

		Script() = default;
		
		void ParseScript(char* fileName);
		
		dx::XMMATRIX DrawScript();

		void Close();

		struct functionS
		{
			dx::XMMATRIX (*foo)(float x, float y, float z) = nullptr;
			float *x = new float(0.0f), *y = new float(0.0f), *z = new float(0.0f);
			bool rep = 0;
		};


		struct paramsC
		{
			float (*foo)(float, float) = nullptr;
			float* param = new float(0.0f);
			float* val = new float(0.0f);
			float* val2 = new float(0.0f);
		};
		
		void ResetParameters();

		std::string GetName() const;

	private:
		
		float* GetPointerParameter(std::string data);
		const float GetParameter(std::string data) const;

		void MathOperation(std::string data);
		void FunctionReg(std::string data);
		auto IsRegistered(std::string value) -> std::map<std::string, float*>::iterator;

		const float MathSimpleOperationRes(std::string operation, float val1, float val2) const;
		const float TrigonometricOperationRes(std::string operation, float val1) const;

		float (*GetMathFucntion(std::string operation))(float, float);

		bool IsTanSinCos(std::string data);
		bool IsLog(std::string data);

		static std::string ExeptionTextCreator(std::string text);
		
		paramsC ParseParameterChanger(std::string operation, std::string val1, 
								   	  std::string val2, std::string res);

	

		bool parsed = false;
		
		std::list<functionS> parsedFunctions;
		std::vector<paramsC> parameterChanger;
		std::map<std::string, float*> parameters;
		std::map<std::string, float> bparameters;
		std::ifstream file;
		std::string fileName;
	};


	namespace SCRIPTS_DEF
	{
		dx::XMMATRIX Rotate(float x, float y, float z);
		dx::XMMATRIX Move(float x, float y, float z);

		float tan(float val1, float val2);
		float atan(float val1, float val2);
		float cotan(float val1, float val2);
		float acotan(float val1, float val2);
		float acos(float val1, float val2);
		float cos(float val1, float val2);
		float sin(float val1, float val2);
		float asin(float val1, float val2);
		float log(float val1, float val2);
		float ln(float val1, float val2);
		float abs(float val1, float val2);
		float sum(float val1, float val2);
		float mul(float val1, float val2);
		float div(float val1, float val2);
		float sub(float val1, float val2);
		float pow(float val1, float val2);
		float remnant(float val1, float val2);
	}





}