#include "pch.h"

namespace BJEUtils {

	std::wstring aiStringToWString(aiString aiStr)
	{
		std::string s = aiStr.C_Str();
		std::wstring wsTmp(s.begin(), s.end());
		std::wstring ws = wsTmp;
		return ws;
	}


}