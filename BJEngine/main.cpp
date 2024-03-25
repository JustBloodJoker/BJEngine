#include <iostream> 
#include "FWork.h"


int main()
{
	SetConsoleCP(1251);
	SetConsoleOutputCP(1251);

	BJFrameWork::FWork frame;
	BJEngine::Render* render = new BJEngine::Render();

	frame.SetRender(render);

	frame.Init(L"BJEngine", L"World", 1920, 1080, BJEUtils::_WINDOWED);
	
	frame.Run();

	frame.Close();


	return 0;
}