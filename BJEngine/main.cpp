#include <iostream>
#include "FWork.h"


int main()
{

	BJFrameWork::FWork frame;
	BJEngine::Render* render = new BJEngine::Render();

	frame.SetRender(render);

	frame.Init(L"BJEngine", L"World", 1600, 1200, BJEUtils::_WINDOWED);
	
	frame.Run();

	frame.Close();


	return 0;
}