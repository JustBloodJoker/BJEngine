#include"Utilites.h"

namespace BJEUtils 
{

	float W, H;
	bool F;
	bool isResized;

	float GetWindowWidth()
	{
		return W;
	}
	float GetWindowHeight()
	{
		return H;
	}

	bool GetFullScreenState()
	{
		return F;
	}

	bool GetIsResizedState()
	{
		return isResized;
	}

	void SetFullScreenState(bool tF)
	{
		if (F != tF)
			F = tF;
	}

	void SetWindowWidth(float tW)
	{
		W = tW;
	}

	void SetWindowHeight(float tH)
	{
		H = tH;
	}

	void SetIsResizedState(bool isRes)
	{
		isResized = isRes;
	}


	std::wstring aiStringToWString(aiString aiStr)
	{
		std::string s = aiStr.C_Str();
		std::wstring wsTmp(s.begin(), s.end());
		std::wstring ws = wsTmp;
		return ws;
	}

	std::wstring charToWString(const char* cch)
	{
		std::string s = std::string(cch);
		std::wstring wsTmp(s.begin(), s.end());
		std::wstring ws = wsTmp;
		return ws;
	}

	std::string ParseLightTypeToStr(int type)
	{
		if (type == POINTLIGHT)
			return "point light";
		else if (type == SPOTLIGHT)
			return "spot light";
		else if (type == DIRECTIONALLIGHT)
			return "directional light";
		else
			return "";
	}


}